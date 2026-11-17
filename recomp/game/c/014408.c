#include "game.h"

/* The character loop of TextMap_CmdText ($014408-$01443F), entered at the character
 * (TextMap_PutChars) or at its dbra step (TextMap_PutCharsNext: the first character of a run, and
 * the typewriter's next one). */

/* the loop of both entries; `put` = start by writing a character (else at the dbra) */
static void put_chars(bool put, uint32_t *d0, uint16_t base, uint16_t *count, const u8 **list, uint32_t *a1,
                      uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *font, uint32_t *dst, dk::Flags *ccr)
{
    PTR(const u16) map;                      /* a word per ASCII code from $20 */
    map.v = *font;
    for (;;) {
        if (put) {
            uint8_t c = *(*list)++;
            uint32_t tile = (uint32_t)(uint16_t)map.get()[c - 0x20] + base;   /* -$40(a5,d0.w): c*2 - $40 */
            *d0 = (uint16_t)tile;            /* moveq #0 first: the upper word 0 */
            PTR(u16) out;
            out.v = *dst;
            *out.get() = (uint16_t)tile;
            *dst += 2;
            if (g_TextMapTyping) {
                /* typewriter: one character per due step; the run is saved and the caller's a1-a6
                   come back. The flags are the move.l a6's; X the add.w's carry. */
                g_TextMapTypeSrc = dk::addr(*list);
                g_TextMapTypeCount = *count;
                g_TextMapTypeDst = *dst;
                *ccr = dk::Flags::test(*dst, 4, tile > 0xFFFFu);
                *a1 = dk::pop_long();        /* movem.l (a7)+,a1-a6 */
                *a2 = dk::pop_long();
                *a3 = dk::pop_long();
                *a4 = dk::pop_long();
                *font = dk::pop_long();
                *dst = dk::pop_long();
                return;
            }
        }
        put = true;
        if ((*count)-- != 0)                 /* dbra d7 */
            continue;
        /* the run's end: the list realigned to a word */
        uint16_t at = (uint16_t)dk::addr(*list);
        *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(at >> 1);
        if (at & 1)
            (*list)++;
        dk::jump(at_TextMap_NextCmd);
        return;
    }
}

/* TextMap_PutChars - writes the next character of a text run: its font map entry (from $20) plus
 * the tile base into the plane image; in typewriter mode it saves the run after that one
 * character and returns to TextMap_Run's caller, else loops.
 * In:  list = the characters, count = left - 1 (dbra), font = the font map, base = the tile base,
 *      dst = the write pointer; the six longs TextMap_Run pushed
 * Out: dst advanced; typewriter: g_TextMapTypeSrc / Count / Dst saved, a1-a6 popped, the flags of
 *      the move.l a6; else at the run's end the list word-aligned, then TextMap_NextCmd
 * Evidence: moveq #0,d0 / move.b (a0)+,d0 / add.w d0,d0 / move.w -$40(a5,d0.w),d0 / add.w d6,d0 /
 *           move.w d0,(a6)+ / tst.b (g_TextMapTyping).w / beq; move.l a0,$F3C0 / move.w d7,$F3BE /
 *           move.l a6,$F3B6 / movem.l (a7)+,a1-a6 / rts; font map Tbl_0586F2
 */
void TextMap_PutChars(uint32_t *d0, uint16_t *base, uint16_t *count, const u8 **list, uint32_t *a1, uint32_t *a2,
                      uint32_t *a3, uint32_t *a4, uint32_t *font, uint32_t *dst, dk::Flags *ccr)
{
    put_chars(true, d0, *base, count, list, a1, a2, a3, a4, font, dst, ccr);
}

/* TextMap_PutCharsNext - the step of the TextMap_PutChars loop: the next character while the dbra
 * count lasts, else the list realigned to a word and the next command.
 * In:  count = the characters left (the dbra counts one off first), list = the next character
 * Out: as TextMap_PutChars
 * Evidence: dbra d7,TextMap_PutChars / move.w a0,d0 / lsr.w #1,d0 / bcc TextMap_NextCmd / addq.l
 *           #1,a0 / bra TextMap_NextCmd; entered by TextMap_CmdText and TextMap_ResumeTyping
 */
void TextMap_PutCharsNext(uint32_t *d0, uint16_t *base, uint16_t *count, const u8 **list, uint32_t *a1,
                          uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *font, uint32_t *dst, dk::Flags *ccr)
{
    put_chars(false, d0, *base, count, list, a1, a2, a3, a4, font, dst, ccr);
}
