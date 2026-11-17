#include "game.h"

enum {
    FLOOR_BOTTOM = 216,          /* the floor's lines are written upward from this one (exclusive) */
    FLOOR_PERSPECTIVE = 80,      /* lines 215-136: one value each, spread by the step */
    FLOOR_FAR = 16,              /* lines 135-120: the far edge, one value */
    STEP_DIVISOR = 0x160,
    FLOOR_BASE = (int32_t)0xFEE08000,  /* -$11F.8 px: the nearest line's x */
    ODD_SEGMENT_SHIFT = 0x300000,      /* odd segments: the far edge $30 px further on */
};

static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }

/* divs.w: quotient in the low word, remainder in the high word; on overflow the register stays */
static uint32_t divs_w(uint32_t d, int16_t s)
{
    int32_t q = (int32_t)d / s, r = (int32_t)d % s;
    if (q < -32768 || q > 32767) return d;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

/* Stage4_LabFloorLines - plane A H-scroll of the lab floor: lines 215 up to 136 spread by a
 * per-line step from the segment offset (the perspective), lines 135-120 one value ($30 px further
 * on odd segments). The block moves up with the shake.
 * In:  g_SceneVars (LabScrollVars: offset, segment), g_ShakeY
 * Out: g_HScrollLinesA lines (120-215) - shake; vars->segment masked to its parity (andi.w in
 *      place); d0 = the last value, its halves swapped (16.16 with the pixels in the low word)
 * Evidence: lea g_HScrollLinesA+$1B0,a2 / suba.w 2*shake / divs.w #$160 / asl.l #1 / addi.l
 *           #$FEE08000 / 80 x swap, move.w -(a2), sub.l / andi.w #1,g_SceneVars+8 / subi.l
 *           #$300000 / 16 x move.w -(a2); only caller Stage4_LabScroll; TAS 119416-122651
 */
uint32_t Stage4_LabFloorLines(void)
{
    LabScrollVars *vars = (LabScrollVars *)g_SceneVars;
    int16_t shake = (int16_t)(uint16_t)((uint32_t)g_ShakeY >> 16);
    u16 *line = (u16 *)g_HScrollLinesA + FLOOR_BOTTOM - shake;
    uint32_t x = (uint32_t)(int32_t)vars->offset;
    uint32_t step = (uint32_t)((int32_t)(int16_t)(uint16_t)divs_w(x, STEP_DIVISOR) << 1);
    x += (uint32_t)FLOOR_BASE;
    for (int i = 0; i < FLOOR_PERSPECTIVE; i++) {
        *--line = (uint16_t)(x >> 16);
        x -= step;
    }
    vars->segment = (uint16_t)(vars->segment & 1);
    if (vars->segment != 0)
        x -= ODD_SEGMENT_SHIFT;
    x = swap(x);
    for (int i = 0; i < FLOOR_FAR; i++)
        *--line = (uint16_t)x;
    return x;
}
