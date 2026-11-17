#include "game.h"

/* VM6_SetupClearRect - a6 helper: sets up a plane fill job: the fill tile $5FC, the VDP write
 * command of the rectangle's first row and the column / row counts (0 = the plane's width /
 * height in cells).
 * In:  d0 = x (bytes), d1 = row, d2 = width, d3 = height; a6 = script: word address of the plane
 *      VRAM variable
 * Out: g_A6TileWord = $5FC; d1 = g_A6VdpCmd = the VRAM write command of base + x + row *
 *      g_PlaneRowBytes; d2 = g_A6RectCols = width - 1, d3 = g_A6RectRows = height - 1; a0 = the
 *      plane variable's address; a6 past the word = g_A6ScriptPc
 * Evidence: move.w #$5FC,(g_A6TileWord).w / movea.w (a6)+,a0 / add.w (a0),d0 / mulu.w
 *           (g_PlaneRowBytes).w,d1 / add.w d0,d1 / lsl.l #2 / lsr.w #2 / ori.w #$4000 / swap; callers
 *           Op6_ClearPlane / Op6_ClearPlaneRect; 2 corpus runs
 */
void VM6_SetupClearRect(uint16_t d0, uint32_t *d1, uint16_t *d2, uint16_t *d3, uint32_t *a0, uint32_t *a6)
{
    g_A6TileWord = 0x5FC;
    PTR(const u16) pc;
    pc.v = *a6;
    const u16 *script = pc;
    *a0 = (uint32_t)(int32_t)(int16_t)vm6_word(&script);
    PTR(const u16) plane_var;
    plane_var.v = *a0;
    d0 = (uint16_t)(d0 + *plane_var);
    uint32_t at = (uint32_t)(uint16_t)*d1 * (uint16_t)g_PlaneRowBytes;   /* mulu.w: a long */
    at = (at & 0xFFFF0000u) | (uint16_t)(at + d0);                       /* add.w */
    *d1 = vm6_vram_write_cmd(at);
    g_A6VdpCmd = *d1;
    if (*d2 == 0)
        *d2 = g_PlaneCellsX;
    if (*d3 == 0)
        *d3 = g_PlaneCellsY;
    *d2 = (uint16_t)(*d2 - 1);
    g_A6RectCols = *d2;
    *d3 = (uint16_t)(*d3 - 1);
    g_A6RectRows = *d3;
    *a6 = ADDR68(script);
    g_A6ScriptPc = *a6;
}
