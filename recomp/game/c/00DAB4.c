#include "game.h"

/* The dithered span fills, part 0 ($00DAB4-$00EAA1): Span_FillRampsDither0Run, its entry
 * Span_FillRampsDither0, and Span_FillRampsDither1Odd (the odd-count half of
 * Span_FillRampsDither1, which jumps into this run).
 *
 * The original is an unrolled run of 18-byte units, one per line: the H-scroll word is the 16.16
 * ramp d0's integer part with bit 0 forced (and.w d5 = $FFFE clears it, or.w d6 = 1 sets it, the
 * two alternating unit by unit), the V-scroll word ramp d2's integer part. Units alternate
 * and/or from the run's start ($00DAB4, and) to its last ($00E7DA, or). The entries jump in a
 * computed count of units before the rts ($00E7EC) or before the last unit, so the first line
 * gets the parity the caller wants; odd counts of Span_FillRampsDither0 go through the twin run
 * Span_FillRampsDither1Run (or/and swapped) the same way. Either run has 36 more bytes-identical
 * units before its start (raw bytes $00D82C-$00DAB3, $00E81A-$00EAA1), so up to 224 lines write
 * the same pattern. The C loops over the same writes, in the same order. */

static constexpr int RUN_UNITS = (0x00E7EC - 0x00DAB4) / 18;   /* 188 */
static constexpr int EXTRA_UNITS = (0x00DAB4 - 0x00D82C) / 18;  /* 36, never reached in the corpus */
static constexpr uint16_t DITHER_CLEAR = 0xFFFE;                  /* d5: and.w = bit 0 clear */
static constexpr uint16_t DITHER_SET = 0x0001;                    /* d6: or.w = bit 0 set */

/* The ramps a fill advances, and the H-scroll long it computed last (d4). */
struct DitherRamps {
    uint32_t h, h_step;     /* d0, d1 */
    uint32_t v, v_step;     /* d2, d3 */
    uint32_t last;          /* d4 */
};

/* `n` lines; the first H-scroll word and.w `clear` if set_first is false, else or.w `set`, then
 * alternating. Each line: move.l d0,d4 / swap d4 / and.w d5,d4 (or.w d6,d4) / move.w d4,(a0)+ /
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

static void check_units(uint16_t displacement, int n, int max, const char *entry)
{
    if ((int16_t)displacement > 0 || n > max)
        dk::fatal("%s: %d lines run past the unrolled run's start", entry, n);
}

/* Span_FillRampsDither0Run - the unrolled run entered at its start: all 188 lines, the first
 * H-scroll word and.w clear (bit 0 clear with the callers' $FFFE), then or.w set, alternating.
 * In:  h/h_step = the H-scroll ramp (d0/d1), v/v_step = the V-scroll ramp (d2/d3, 16.16);
 *      clear/set = the H-scroll masks (d5/d6); dst_h/dst_v = the line tables (a0/a1)
 * Out: dst_h/dst_v past the lines; h/v advanced 188 steps; last = the last H-scroll long (d4)
 * Evidence: 94 x the and.w d5 / or.w d6 unit pair, then the rts at $00E7EC; entered only by the
 *           computed jumps at $00E806 / $00E816 (a count of 188 starts here)
 */
void Span_FillRampsDither0Run(uint32_t *h, uint32_t h_step, uint32_t *v, uint32_t v_step, uint32_t *last,
                              uint16_t clear, uint16_t set, uint32_t *dst_h, uint32_t *dst_v)
{
    DitherRamps r = { *h, h_step, *v, v_step, *last };
    fill_dithered(&r, clear, set, false, RUN_UNITS, dst_h, dst_v);
    *h = r.h;
    *v = r.v;
    *last = r.last;
}

/* Span_FillRampsDither0 - Span_FillRamps for the dithered span effect: the first line's H-scroll
 * gets bit 0 clear, the next bit 0 set, and so on, keeping a checkerboard plane image aligned line
 * to line.
 * In:  h/h_step, v/v_step = the 16.16 ramps (d0/d1, d2/d3); lines = d7.w (0-224); dst_h/dst_v =
 *      the H/V line tables (a0/a1)
 * Out: dst_h/dst_v past the lines; h/v advanced; clear = $FFFE (d5.w); lines = -36 x (lines / 2)
 *      in its low word (the jump's displacement; lsr.w keeps the high word); last = the last
 *      H-scroll long (d4), or with no lines its low word 4 x (lines / 2) = 0; d6 = 1 (clobbered)
 * Evidence: move.w #$FFFE,d5 / moveq #1,d6 / lsr.w #1,d7 / bcs.w loc_00F7F8 (odd: into
 *           Span_FillRampsDither1Run before its last unit, an and.w) / d7 x 36 / jmp
 *           loc_00E7EC(pc,d7.w); callers Span_DrawTriangleDither by line and frame parity;
 *           executed only in fuzz-77400-s1
 */
void Span_FillRampsDither0(uint32_t *h, uint32_t *h_step, uint32_t *v, uint32_t v_step, uint32_t *last,
                           uint16_t *clear, uint32_t *lines, uint32_t *dst_h, uint32_t *dst_v)
{
    *clear = DITHER_CLEAR;
    uint16_t count = (uint16_t)*lines;
    uint16_t pairs = (uint16_t)(count >> 1);
    bool odd = (count & 1) != 0;
    uint16_t d7 = displacement_of(pairs);
    *lines = (*lines & 0xFFFF0000u) | d7;
    *last = (*last & 0xFFFF0000u) | (uint16_t)(pairs * 4);   /* move.w d7,d4 (d7 = 4p) */
    int n = 2 * (-(int16_t)d7 / 36) + (odd ? 1 : 0);
    check_units(d7, n, RUN_UNITS + EXTRA_UNITS - (odd ? 1 : 0), "Span_FillRampsDither0");
    DitherRamps r = { *h, *h_step, *v, v_step, *last };
    fill_dithered(&r, DITHER_CLEAR, DITHER_SET, false, n, dst_h, dst_v);
    *h = r.h;
    *v = r.v;
    *last = r.last;
}

/* Span_FillRampsDither1Odd - the odd-count half of Span_FillRampsDither1: 2p+1 lines through this
 * run's last unit, so the first line's H-scroll gets bit 0 set (or.w), then clear, alternating.
 * In:  pairs = p = lines / 2 (d7.w, after Span_FillRampsDither1's lsr); clear/set = $FFFE / 1
 *      (d5/d6); h/h_step, v/v_step = the ramps (d0/d1, d2/d3); dst_h/dst_v = the line tables
 * Out: dst_h/dst_v past the lines; h/v advanced; pairs = -36p in its low word; last = the last
 *      H-scroll long (d4)
 * Evidence: bcs.w from Span_FillRampsDither1 ($00F7E4); d7 x 36 / jmp loc_00E7DA(pc,d7.w): the
 *           last unit (or.w d6) and the 2p before it
 */
void Span_FillRampsDither1Odd(uint32_t *h, uint32_t h_step, uint32_t *v, uint32_t v_step, uint32_t *last,
                              uint16_t clear, uint16_t set, uint32_t *pairs, uint32_t *dst_h, uint32_t *dst_v)
{
    uint16_t p = (uint16_t)*pairs;
    uint16_t d7 = displacement_of(p);
    *pairs = (*pairs & 0xFFFF0000u) | d7;
    int n = 2 * (-(int16_t)d7 / 36) + 1;
    check_units(d7, n, RUN_UNITS + EXTRA_UNITS, "Span_FillRampsDither1Odd");
    DitherRamps r = { *h, h_step, *v, v_step, *last };
    fill_dithered(&r, clear, set, true, n, dst_h, dst_v);
    *h = r.h;
    *v = r.v;
    *last = r.last;
}
