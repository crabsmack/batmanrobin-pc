#include "game.h"

/* The dithered span fills, part 1 ($00EAA2-$00F807): Span_FillRampsDither1Run, its entry
 * Span_FillRampsDither1, and Span_FillRampsDither0Odd (the odd-count half of
 * Span_FillRampsDither0, which jumps into this run).
 *
 * The twin of 00DAB4.c's run with or/and swapped: 188 units of 18 bytes, one per line, the
 * H-scroll word the 16.16 ramp d0's integer part with bit 0 set (or.w d6 = 1) or clear
 * (and.w d5 = $FFFE), alternating from the run's start ($00EAA2, or) to its last unit ($00F7C8,
 * and); the V-scroll word ramp d2's integer part. Span_FillRampsDither1's even counts jump in
 * 2p units before the rts ($00F7DA), Span_FillRampsDither0's odd ones 2p+1 units before it
 * (through the last unit, `jmp $F7C8(pc,d7.w)`); Span_FillRampsDither1's odd counts go through
 * Span_FillRampsDither1Odd into the twin run. 36 more bytes-identical units sit before the run
 * ($00E81A-$00EAA1, never reached in the corpus), so up to 224 lines write the same pattern. The
 * C loops over the same writes, in the same order. */

static constexpr int RUN_UNITS = (0x00F7DA - 0x00EAA2) / 18;    /* 188 */
static constexpr int EXTRA_UNITS = (0x00EAA2 - 0x00E81A) / 18;  /* 36, never reached in the corpus */
static constexpr uint16_t DITHER_CLEAR = 0xFFFE;                  /* d5: and.w = bit 0 clear */
static constexpr uint16_t DITHER_SET = 0x0001;                    /* d6: or.w = bit 0 set */

/* The ramps a fill advances, and the H-scroll long it computed last (d4). */
struct DitherRamps {
    uint32_t h, h_step;     /* d0, d1 */
    uint32_t v, v_step;     /* d2, d3 */
    uint32_t last;          /* d4 */
};

/* `n` lines; the first H-scroll word or.w `set` if set_first, else and.w `clear`, then
 * alternating. Each line: move.l d0,d4 / swap d4 / or.w d6,d4 (and.w d5,d4) / move.w d4,(a0)+ /
 * add.l d1,d0 / swap d2 / move.w d2,(a1)+ / swap d2 / add.l d3,d2 */
static void fill_dithered(DitherRamps *r, uint16_t clear, uint16_t set, bool set_first, int n,
                          uint32_t *dst_h, uint32_t *dst_v)
{
    PTR(u16) ph, pv;
    ph.v = *dst_h;
    pv.v = *dst_v;
    u16 *h = ph, *v = pv;
    bool set_bit = set_first;
    for (int i = 0; i < n; i++, set_bit = !set_bit) {
        uint32_t w = r->h << 16 | r->h >> 16;
        w = set_bit ? (w | set) : (w & (0xFFFF0000u | clear));
        r->last = w;
        *h++ = (uint16_t)w;
        r->h += r->h_step;
        *v++ = (uint16_t)(r->v >> 16);
        r->v += r->v_step;
    }
    *dst_h = ADDR68(h);
    *dst_v = ADDR68(v);
}

/* the computed jump: d7 = p (lines / 2) becomes -36p, the displacement back from its target */
static uint16_t displacement_of(uint16_t pairs)
{
    return (uint16_t)-(uint16_t)(pairs * 36);
}

static void check_units(int n, int max, const char *entry)
{
    if (n > max)
        dk::fatal("%s: %d lines run past the unrolled run's start", entry, n);
}

/* Span_FillRampsDither1Run - the unrolled run entered at its start: all 188 lines, the first
 * H-scroll word or.w set (bit 0 set with the callers' 1), then and.w clear, alternating.
 * In:  h/h_step = the H-scroll ramp (d0/d1), v/v_step = the V-scroll ramp (d2/d3, 16.16);
 *      clear/set = the H-scroll masks (d5/d6); dst_h/dst_v = the line tables (a0/a1)
 * Out: dst_h/dst_v past the lines; h/v advanced 188 steps; last = the last H-scroll long (d4)
 * Evidence: 94 x the or.w d6 / and.w d5 unit pair, then the rts at $00F7DA; entered only by the
 *           computed jumps at $00F7F4 / $00F804 (dkit.toml computed_entry_runs; 188 lines start
 *           here)
 */
void Span_FillRampsDither1Run(uint32_t *h, uint32_t h_step, uint32_t *v, uint32_t v_step, uint32_t *last,
                              uint16_t clear, uint16_t set, uint32_t *dst_h, uint32_t *dst_v)
{
    DitherRamps r = { *h, h_step, *v, v_step, *last };
    fill_dithered(&r, clear, set, true, RUN_UNITS, dst_h, dst_v);
    *h = r.h;
    *v = r.v;
    *last = r.last;
}

/* Span_FillRampsDither1 - Span_FillRampsDither0 with the opposite phase: the first line's
 * H-scroll gets bit 0 set, the next bit 0 clear, and so on.
 * In:  h/h_step, v/v_step = the 16.16 ramps (d0/d1, d2/d3); lines = d7.w (0-224); dst_h/dst_v =
 *      the H/V line tables (a0/a1)
 * Out: dst_h/dst_v past the lines; h/v advanced; clear = $FFFE (d5.w); lines = -36 x (lines / 2)
 *      in its low word (the jump's displacement; lsr.w keeps the high word); last = the last
 *      H-scroll long (d4), or with no lines its low word 4 x (lines / 2) = 0; d6 = 1 (clobbered)
 * Evidence: move.w #$FFFE,d5 / moveq #1,d6 / lsr.w #1,d7 / bcs.w Span_FillRampsDither1Odd (odd:
 *           into Span_FillRampsDither0Run before its last unit, an or.w) / d7 x 36 / jmp
 *           loc_00F7DA(pc,d7.w); callers Span_DrawTriangleDither by line and frame parity
 */
void Span_FillRampsDither1(uint32_t *h, uint32_t *h_step, uint32_t *v, uint32_t v_step, uint32_t *last,
                           uint16_t *clear, uint32_t *lines, uint32_t *dst_h, uint32_t *dst_v)
{
    *clear = DITHER_CLEAR;
    uint16_t count = (uint16_t)*lines;
    uint16_t pairs = (uint16_t)(count >> 1);
    bool odd = (count & 1) != 0;
    uint16_t d7 = displacement_of(pairs);
    *lines = (*lines & 0xFFFF0000u) | d7;
    *last = (*last & 0xFFFF0000u) | (uint16_t)(pairs * 4);   /* move.w d7,d4 (d7 = 4p) */
    int n = 2 * pairs + (odd ? 1 : 0);
    check_units(n, RUN_UNITS + EXTRA_UNITS, "Span_FillRampsDither1");   /* odd: the twin run's 224 */
    DitherRamps r = { *h, *h_step, *v, v_step, *last };
    fill_dithered(&r, DITHER_CLEAR, DITHER_SET, true, n, dst_h, dst_v);
    *h = r.h;
    *v = r.v;
    *last = r.last;
}

/* Span_FillRampsDither0Odd - the odd-count half of Span_FillRampsDither0: 2p+1 lines through this
 * run's last unit, so the first line's H-scroll gets bit 0 clear (and.w), then set, alternating.
 * In:  pairs = p = lines / 2 (d7.w, after Span_FillRampsDither0's lsr); clear/set = $FFFE / 1
 *      (d5/d6); h/h_step, v/v_step = the ramps (d0/d1, d2/d3); dst_h/dst_v = the line tables
 * Out: dst_h/dst_v past the lines; h/v advanced; pairs = -36p in its low word; last = the last
 *      H-scroll long (d4; its low word 4p first); d1, d5 kept; d6 clobbered
 * Evidence: bcs.w from Span_FillRampsDither0 ($00E7F8); d7 x 36 / jmp loc_00F7C8(pc,d7.w): the
 *           last unit (and.w d5) and the 2p before it
 */
void Span_FillRampsDither0Odd(uint32_t *h, uint32_t *h_step, uint32_t *v, uint32_t v_step, uint32_t *last,
                              uint16_t *clear, uint16_t set, uint32_t *pairs, uint32_t *dst_h, uint32_t *dst_v)
{
    uint16_t p = (uint16_t)*pairs;
    uint16_t d7 = displacement_of(p);
    *pairs = (*pairs & 0xFFFF0000u) | d7;
    *last = (*last & 0xFFFF0000u) | (uint16_t)(p * 4);       /* move.w d7,d4 (d7 = 4p) */
    int n = 2 * p + 1;
    check_units(n, RUN_UNITS + EXTRA_UNITS - 1, "Span_FillRampsDither0Odd");
    DitherRamps r = { *h, *h_step, *v, v_step, *last };
    fill_dithered(&r, *clear, set, false, n, dst_h, dst_v);
    *h = r.h;
    *v = r.v;
    *last = r.last;
}
