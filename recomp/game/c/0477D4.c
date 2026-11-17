#include "game.h"

enum {
    SEGMENT_DIVISOR = 0x5700,           /* (floor_x >> 8) / this: an 87-px floor segment */
    UPPER_FACTOR = 0x180,
    UPPER_OFFSET = 0x5F0000,
    UPPER_WRAP = 0x8F0000,              /* the upper wall repeats every $8F px */
    UPPER_HIGH = (int32_t)0xFFE00000,   /* kept within (FF50, FFE0) px */
    UPPER_LOW = (int32_t)0xFF500000,
};

/* divu.w: quotient in the low word, remainder in the high word; on overflow the register stays */
static uint32_t divu_w(uint32_t d, uint16_t s)
{
    uint32_t q = d / s, r = d % s;
    if (q > 0xFFFFu) return d;
    return (r << 16) | q;
}

/* Stage4_LabScroll - the lab's scroll every frame: both planes' V-scroll = g_ViewY; the floor's x
 * = g_ViewX * 35/32, split into an 87-px segment number and the offset within it; then the floor
 * lines, the upper wall's lines (based $A px on from the floor's last line, wrapped into one
 * repeat) and plane B's lines.
 * In:  g_ViewX, g_ViewY; the line routines' registers
 * Out: g_VScrollA/B; g_SceneVars (LabScrollVars: floor_x, offset, segment, upper); the line buffers;
 *      d1-d6, a2, a3 = the line routines'
 * Evidence: lsr.l #3 / add / lsr.l #2 / sub (x * 35/32) / lsr.l #8 / divu.w #$5700 / clr.w / lsr.l
 *           #8 / neg.l / bsr Stage4_LabFloorLines / swap / asr.l #8 / muls.w #$180 / subi.l #$5F0000
 *           / the $8F0000 wrap / bsr Stage4_LabUpperLines / bra.w Stage4_LabPlaneBLines; only caller
 *           Stage4_RunLab's frame loop; TAS 119416-122651
 */
void Stage4_LabScroll(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2,
                      uint32_t *a3)
{
    LabScrollVars *vars = (LabScrollVars *)g_SceneVars;
    uint16_t view_y = (uint16_t)((uint32_t)g_ViewY >> 16);
    g_VScrollA = view_y;
    g_VScrollB = view_y;

    uint32_t x = (uint32_t)g_ViewX;
    uint32_t eighth = x >> 3;
    x += eighth;
    x -= eighth >> 2;                              /* x + x/8 - x/32 = x * 35/32 */
    vars->floor_x = x;
    uint32_t split = divu_w(x >> 8, SEGMENT_DIVISOR);
    vars->segment = (uint16_t)split;
    vars->offset = (int32_t)(0u - ((split & 0xFFFF0000u) >> 8));

    uint32_t last = Stage4_LabFloorLines();        /* its halves swapped: back to 16.16 here */
    last = (last << 16) | (last >> 16);
    int32_t upper = (int32_t)(int16_t)(uint16_t)((int32_t)last >> 8) * UPPER_FACTOR;
    upper -= UPPER_OFFSET;
    if (upper >= UPPER_HIGH)
        upper -= UPPER_WRAP;
    if (upper <= UPPER_LOW)
        upper += UPPER_WRAP;
    vars->upper = upper;

    Stage4_LabUpperLines(d3, d4, d5, d6, a2, a3);
    dk::jump(at_Stage4_LabPlaneBLines);
}
