#include "game.h"

/* The decoder's state is the asm's registers, kept as they are because the caller sees them (the
 * prototype's outputs) and because the bit reader's arithmetic is the format: d1 is the bit buffer
 * (its high word the unread bits, shifted out at the top; its low word where a field lands), d7
 * the bits left in the buffer's word minus 1, d6 a field's width, d2 a match's length - 1. */
struct LzState {
    uint32_t d1, d2, d6, d7;
    const u8 *stream;   /* a0: the control bit stream (a long offset to the literals, then words) */
    const u8 *lits;     /* a2: the literal bytes */
    u8 *out;            /* a1 */
    const u8 *from;     /* a3: a match's source in the output */

    static uint16_t lo(uint32_t r) { return (uint16_t)r; }
    static uint32_t set_lo(uint32_t r, uint16_t w) { return (r & 0xFFFF0000u) | w; }
    static uint32_t swap(uint32_t r) { return r << 16 | r >> 16; }
    static uint32_t rol(uint32_t r, unsigned n) { n &= 31; return n ? (r << n | r >> (32 - n)) : r; }

    uint16_t next_word()
    {
        uint16_t w = (uint16_t)(stream[0] << 8 | stream[1]);
        stream += 2;
        return w;
    }
    /* move.w (a0)+,d1 / swap d1 / moveq #$F,d7: sixteen fresh bits at the top of the buffer */
    void refill()
    {
        d1 = swap(set_lo(d1, next_word()));
        d7 = 0xF;
    }
    /* dbra d7: false when the buffer's word ran out */
    bool count_down()
    {
        d7 = set_lo(d7, (uint16_t)(lo(d7) - 1));
        return lo(d7) != 0xFFFFu;
    }
    /* add.l d1,d1: the next bit (the carry) */
    bool bit()
    {
        bool c = (d1 & 0x80000000u) != 0;
        d1 <<= 1;
        return c;
    }
    /* moveq #n,d6 / clr.w d1 / sub.w d6,d7 / bgt / ... / rol.l d6,d1: the next n bits into d1's low
       word, refilling the buffer's word in the middle of the field when it runs out */
    uint16_t field(int n)
    {
        d6 = (uint32_t)n;
        d1 = set_lo(d1, 0);
        int16_t left = (int16_t)lo(d7);
        d7 = set_lo(d7, (uint16_t)(lo(d7) - lo(d6)));
        if (!(left > (int16_t)lo(d6))) {   /* bgt after sub.w: the field reaches past the word */
            d7 = set_lo(d7, (uint16_t)(lo(d7) + lo(d6)));
            d6 = set_lo(d6, (uint16_t)(lo(d6) - lo(d7)));
            d1 = rol(d1, lo(d7) & 63);         /* the bits this word still has */
            d1 = swap(set_lo(swap(d1), next_word()));
            d7 = 0x10;
            d7 = set_lo(d7, (uint16_t)(lo(d7) - lo(d6)));
        }
        d1 = rol(d1, lo(d6) & 63);
        return lo(d1);
    }
    /* move.b (a3)+,(a1)+ / dbra d2: a match of d2.w + 1 bytes from `back` bytes before the output */
    void copy(uint16_t back)
    {
        from = out - (int16_t)back;            /* movea.l a1,a3 / suba.w d1,a3 */
        do
            *out++ = *from++;
        while ((d2 = set_lo(d2, (uint16_t)(lo(d2) - 1)), lo(d2) != 0xFFFFu));
    }
};

/* Lz_Decompress - the LZ77 decompressor (the background task's code, and called directly by the
 * a6 op $7108): a control bit stream whose 0 bits copy a literal byte from a separate stream and
 * whose 1 bits start a match (a length code, then a distance class and offset) copied from the
 * output already written, until the length code 0 of the 8-bit class. Then it ends the background
 * task.
 * In:  a0 = the packed data (a long: the literals' offset from a0, then the control words);
 *      a1 = the destination
 * Out: g_LzOutEnd = a1 = the output's end; g_BgTaskActive, g_BgTaskRunning clear; the interrupt
 *      mask 0 (masked around the clears); d1, d2, d6, d7, a0, a2, a3 as the decoder leaves them;
 *      the flags of clr.b g_BgTaskRunning
 * Evidence: movea.l a0,a2 / adda.l (a0)+,a2 / bra refill; the literal (move.b (a2)+,(a1)+) and
 *           the dbra d7 / move.w (a0)+,d1 / swap / moveq #$F refill; after a 1 bit: d2 = 1 and a
 *           second bit (0: length 2), else 2-bit (d2 = 1 + it), 4-bit (4 + it), 8-bit ($13 + it,
 *           0 = the end) length fields; the distance class (2 bits) picks a 5 / 7 / 9 / 10-bit
 *           offset field + 1 / $21 / $A1 / $2A1; movea.l a1,a3 / suba.w d1,a3 / the dbra d2 copy;
 *           move.l a1,g_LzOutEnd / ori.w #$700,sr / clr.b x2 / andi.w #$F8FF,sr / rts. Ruling 1
 *           (P4-decomp.md): a plain C function; parking it mid-way is the timed machine's
 *           generated code's (BgTask_Resume's C resumes a parked context there by its RTE)
 */
void Lz_Decompress(uint32_t *d1, uint32_t *d2, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2,
                   uint32_t *a3, dk::Flags *ccr)
{
    /* the addresses as the caller passed them: the outputs are those plus how far each pointer
       moved (a2 and a3 are set before they are read: their inputs are not addresses) */
    const uint32_t a0_in = *a0, a1_in = *a1;
    PTR(const u8) src;
    src.v = a0_in;
    PTR(u8) dst;
    dst.v = a1_in;
    const u8 *src0 = src;
    u8 *dst0 = dst;

    LzState s{ *d1, *d2, *d6, *d7, src, nullptr, dst, nullptr };
    uint32_t lit_ofs = (uint32_t)s.next_word() << 16;
    lit_ofs |= s.next_word();
    s.lits = src0 + (int32_t)lit_ofs;          /* movea.l a0,a2 / adda.l (a0)+,a2 */
    s.refill();
    for (;;) {
        if (!s.bit()) {
            *s.out++ = *s.lits++;              /* a literal */
        } else {
            s.d2 = 1;
            if (!s.count_down())
                s.refill();
            if (s.bit()) {                     /* a length field (else the length is 2) */
                uint16_t len = s.field(2);
                if (!len) {
                    s.d2 = 4;
                    len = s.field(4);
                    if (!len) {
                        s.d2 = 0x13;
                        len = s.field(8);
                        if (!len)
                            break;             /* the end code */
                    }
                }
                s.d2 = LzState::set_lo(s.d2, (uint16_t)(LzState::lo(s.d2) + len));
            }
            uint16_t cls = s.field(2);
            uint16_t back;
            if (cls == 0)
                back = (uint16_t)(s.field(5) + 1);
            else if (cls == 1)
                back = (uint16_t)(s.field(7) + 0x21);
            else if (cls == 2)
                back = (uint16_t)(s.field(9) + 0xA1);
            else
                back = (uint16_t)(s.field(10) + 0x2A1);
            /* d1's low word: the subq / addq / addi the asm did on it */
            s.d1 = LzState::set_lo(s.d1, back);
            s.copy(back);
        }
        if (!s.count_down())
            s.refill();
    }

    *d1 = s.d1;
    *d2 = s.d2;
    *d6 = s.d6;
    *d7 = s.d7;
    *a0 = a0_in + (uint32_t)(s.stream - src0);
    *a1 = a1_in + (uint32_t)(s.out - dst0);
    *a2 = a0_in + (uint32_t)(s.lits - src0);
    if (s.from)                                /* a match was copied: a3 is where it stopped */
        *a3 = a1_in + (uint32_t)(s.from - (const u8 *)dst0);
    g_LzOutEnd = *a1;
    dk::set_interrupt_mask(7);
    g_BgTaskActive = 0;
    g_BgTaskRunning = 0;
    dk::set_interrupt_mask(0);
    *ccr = dk::Flags::test(0, 1, ccr->x);      /* clr.b g_BgTaskRunning */
}
