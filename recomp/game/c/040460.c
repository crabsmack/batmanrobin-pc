#include "game.h"

enum {
    CHECKER_PERIOD = 0x6000,       /* one checker period: $60 px (in 1/256 px) */
    FLOOR_HORIZON_STEP = 0x9F,     /* the ramp's step: 1/159 of the offset per line */
    FLOOR_LINES = 0x50,            /* the floor's ramp lines (Mem_FillRamp) */
    BEND_LINES = 0x28,             /* the foreground lines bent by g_Shake2Pos */
    FLOOR_FIRST_LINE = 0x68,       /* the floor starts at screen line 104 - the view y */
};
static constexpr uint32_t CHECKER_PERIOD_16_16 = 0x600000u;

static uint16_t high_word(uint32_t v) { return (uint16_t)(v >> 16); }

/* divu.w: quotient in the low word, remainder in the high word; on overflow the register stays */
static uint32_t divu_w(uint32_t d, uint16_t s)
{
    uint32_t q = d / s, r = d % s;
    if (q > 0xFFFF)
        return d;
    return (r << 16) | q;
}

/* Stage3_CardHallFloorScroll - per-frame line H-scroll of the card hall's checkerboard floor on
 * plane A: from screen line 104 - the view y, 80 lines of a perspective ramp of the view x (x
 * 1.3125, wrapped to one $60-px checker period, step -1/159 per line toward the horizon), then 40
 * lines continuing it, bent by g_Shake2Pos (step = pos / 256, growing by 1/16 of it a line)
 * In:  d3/a1 = the frame loop's: Mem_FillRamp writes its second ramp (d2 += d3) to a1 too (a bug
 *      of the original: 80 words wherever the world step left a1)
 * Out: g_HScrollLinesA's 120 lines; d0 = the ramp after the last line, d1 = the bend's step, d2 =
 *      its growth, d7 = $FFFF (the dbra), a0 = past the lines, a1 = past the second ramp
 * Evidence: move.l g_ViewX,d0 / lsr.l #2 twice / lsr.l #8 / divu.w #$6000 / clr.w / lsr.l #8 /
 *           cmpi.l #$600000 / divu.w #$9F / andi.l #$FFFF / addi.l #$8000 / neg.l / lea
 *           g_HScrollLinesA+$D0 - 2 x g_ViewY / mulu.w $4F / neg.l d1 / moveq #$50,d7 /
 *           Mem_FillRamp / sub.l d1,d0 / lsr.w #1,d2 / bcc / g_Shake2Pos asr.l #8 / asr.l #4 / the
 *           dbra of $27; bsr from Stage3_RunCardHall's frame loop; TAS f108331-f113271 (4622
 *           frames). d0, d7 and a1 outputs (measured as clobbers): the caller's next call,
 *           Scroll_RunParallax, takes all three (&d0, &d7, &a1) as the asm leaves them
 */
void Stage3_CardHallFloorScroll(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint16_t *d7, uint32_t *a1)
{
    uint32_t x = g_ViewX;
    uint32_t quarter = x >> 2;
    x += quarter;
    quarter >>= 2;
    x += quarter;                                  /* the view x x 1.3125 */
    x = divu_w(x >> 8, CHECKER_PERIOD);
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)x;       /* checker periods passed */
    uint32_t ramp = (x & 0xFFFF0000u) >> 8;        /* the position within one period (16.16) */
    if ((int32_t)ramp >= (int32_t)CHECKER_PERIOD_16_16)
        ramp -= CHECKER_PERIOD_16_16;
    uint32_t step = divu_w(ramp, FLOOR_HORIZON_STEP) & 0xFFFFu;
    ramp = (uint32_t)-(int32_t)(ramp + 0x8000u);
    uint32_t a0 = dk::addr(g_HScrollLinesA + 2 * FLOOR_FIRST_LINE) - (uint32_t)(int32_t)(int16_t)(2 * high_word(g_ViewY));
    ramp += (uint32_t)(FLOOR_LINES - 1) * (uint16_t)step;
    step = (uint32_t)-(int32_t)step;
    *d0 = ramp;
    *d1 = step;
    Mem_FillRamp(d0, *d1, d2, d3, FLOOR_LINES, &a0, a1);
    *d0 -= *d1;
    bool odd = (*d2 & 1) != 0;                     /* lsr.w #1,d2: the carry */
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)((uint16_t)*d2 >> 1);
    if (odd)
        *d0 -= CHECKER_PERIOD_16_16;
    *d1 = (uint32_t)((int32_t)g_Shake2Pos >> 8);
    *d2 = (uint32_t)((int32_t)*d1 >> 4);
    PTR(u16) line;
    line.v = a0;
    for (int i = 0; i < BEND_LINES; i++) {
        line[i] = high_word(*d0);
        *d0 += *d1;
        *d1 += *d2;
    }
    *d7 = 0xFFFF;
}
