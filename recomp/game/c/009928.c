#include "game.h"

/* divs.w: the 32-bit dividend by a signed word; the remainder in the high word, the quotient in the
   low. A quotient that doesn't fit a word sets V and leaves the dividend unchanged. */
static uint32_t divs_w(uint32_t dividend, int16_t divisor)
{
    if (divisor == 0)
        dk::fatal("Scroll_FillBands: divs.w by zero (a band divisor of 0)");
    int64_t q = (int64_t)(int32_t)dividend / divisor;
    if (q < -32768 || q > 32767)
        return dividend;
    int32_t r = (int32_t)dividend % divisor;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

/* Scroll_FillBands - fills the 224 screen lines of a line H-scroll buffer from a band table: the
 * bands above the view (y) are skipped, the first visible one is cut at the screen's top, the last at
 * its bottom, and each band's lines get -(x / its divisor).
 * In:  x (d2) = 16.8 (divisor $100 = 1:1), y (d3) = the view's y; lines (a0) = the buffer;
 *      bands (a1) = (word height, word divisor) pairs, height 0 ends, $7FFF = the rest
 * Out: bands past the last pair read; left (d6) = lines left (<= 0 when the screen filled);
 *      count (d7) = -2 x the last band's lines (Mem_FillWords), or the 0 height that ended it;
 *      value (d0) = the last divs result with its low word negated
 * Evidence: move.w #$E0,d6 / neg.w d7 / add.w (a1)+,d7 / bgt; the skip loop addq.l #2,a1 / add.w
 *           (a1)+,d7 / ble; sub.w d7,d6 / bpl / add.w d6,d7; divs.w (a1)+,d0 / neg.w / bsr
 *           Mem_FillWords; tables $971C ($7FFF,$100), $9720 ($60,$280 / $20,$180 / $7FFF,$C0)
 */
void Scroll_FillBands(uint32_t *value, uint32_t d1, uint32_t *x, uint32_t y, uint16_t *left, uint16_t *count,
                      u16 *lines, const s16 **bands)
{
    const s16 *band = *bands;
    uint16_t remaining = 0xE0;

    /* the first band reaching below the view's top: its lines on screen (add.w: the signed sum
       decides bgt/ble, overflow included) */
    int32_t below = (int16_t)(uint16_t)(0u - (uint16_t)y) + *band++;
    while (below <= 0) {
        band++;                                       /* the skipped band's divisor */
        below = (int16_t)(uint16_t)below + *band++;
    }
    uint16_t n = (uint16_t)below;

    remaining = (uint16_t)(remaining - n);
    for (;;) {
        if ((int16_t)remaining < 0)
            n = (uint16_t)(n + remaining);            /* the band runs off the screen's bottom */
        *value = divs_w(*x, *band++);
        *value = (*value & 0xFFFF0000u) | (uint16_t)(0u - (uint16_t)*value);
        uint16_t words = n;
        uint32_t dst = dk::addr(lines), a1 = dk::addr(band);
        Mem_FillWords(value, d1, x, y, &n, &dst, &a1);   /* n words of the value; n = -2 x n */
        lines += words;
        if ((int16_t)remaining <= 0)
            break;
        n = (uint16_t)*band++;
        if (n == 0)
            break;
        remaining = (uint16_t)(remaining - n);
    }
    *bands = band;
    *left = remaining;
    *count = n;
}
