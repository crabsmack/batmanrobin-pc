#include "game.h"

/* Span_BuildCircle - builds the half-width table of a circle of radius r for Span_DrawDiscDither:
 * r+1 words at g_CircleSpans, row 0 (the top) to row r (the centre), by the midpoint algorithm.
 * The octant is walked once: x counts up and is stored from the top row down, while y counts down
 * and is stored from the centre row up, so each store lands on its mirrored row.
 * In:  radius (d0.w; its sign ignored)
 * Out: g_CircleSpans[0..r]; err = the error term (d2: the word arithmetic over moveq #0, so the
 *      high word 0), step = the error step (d3: over moveq #-2, the high word $FFFF), dstep (d4.w)
 * Evidence: tst.w d0 / bgt / neg.w d0; lea g_CircleSpans,a0 / lea 2(a0,d0.w),a1 with d0 = 2r;
 *           move.w d0,(a0)+ / move.w d1,-(a1) / addq.w #4,d3 / sub.w d3,d2 / bpl / subq.w #4,d4 /
 *           cmp.w d0,d1 / bge; callers $334D2 (twice, radius $E5F6) before Span_DrawDiscDither /
 *           Span_ClearDrawDisc; TAS frames 77312-79108
 */
void Span_BuildCircle(uint16_t radius, uint32_t *err_out, uint32_t *step_out, uint16_t *dstep_out)
{
    uint16_t r = radius;
    if ((int16_t)r <= 0)
        r = (uint16_t)-r;
    u16 *top = (u16 *)g_CircleSpans;                                       /* a0 */
    u16 *bottom = top + 1 + ((int16_t)(uint16_t)(r * 2) >> 1);   /* a1 = 2(a0,2r.w): past row r */
    uint16_t x = 0;                                                     /* d0 */
    uint16_t y = r;                                                     /* d1 */
    uint16_t step = 0xFFFE;                                             /* d3 */
    uint16_t dstep = (uint16_t)((r << 2) - 2);                          /* d4 */
    uint16_t err = dstep;                                               /* d2 */
    do {
        *top++ = x;
        do {
            *--bottom = y;
            x++;
            step += 4;
            err -= step;
        } while ((int16_t)err >= 0);                                    /* bpl */
        err += dstep;
        dstep -= 4;
        y--;
    } while ((int16_t)y >= (int16_t)x);                                 /* cmp.w d0,d1 / bge */
    *err_out = err;
    *step_out = 0xFFFF0000u | step;
    *dstep_out = dstep;
}
