#include "game.h"

/* Parallax commands $14 and $18 ($009884-$0098FD): plane B's line H-scroll buffer from the view
 * position, the second with an x offset from the list. */

/* the shared tail: plane B's lines from x (16.8) and the view's y, then the next command */
static void fill_plane_b(uint32_t *d0, uint32_t d1, uint32_t *x, uint32_t *y, uint16_t *d6, uint16_t *d7,
                         uint32_t *bands)
{
    *y = (*y & 0xFFFF0000u) | (uint16_t)(g_ViewY >> 16);   /* the word at g_ViewY: the integer part */
    PTR(const s16) b;
    b.v = *bands;
    const s16 *band = b;
    Scroll_FillBands(d0, d1, x, *y, d6, d7, (u16 *)g_HScrollLinesB, &band);
    *bands = dk::addr(band);
    g_VScrollB = (uint16_t)*y;
    dk::jump(at_Scroll_NextCmd);
}

/* ScrollCmd_CameraB - parallax command $14: plane B's line H-scroll buffer filled in bands from the
 * view position (x = g_ViewX as 16.8), g_VScrollB = the view's y; then the next command.
 * In:  list (a5) = a long band table; g_ViewX, g_ViewY
 * Out: g_HScrollLinesB filled; g_VScrollB; bands (a1) past the pairs used; list past the operand;
 *      x (d2) / y (d3) and the fill's d0/d6/d7
 * Evidence: lea g_HScrollLinesB,a0 / movea.l (a5)+,a1 / move.l g_ViewX,d2 / asr.l #8 / move.w
 *           g_ViewY,d3 / bsr Scroll_FillBands / move.w d3,$DFB8 / bra Scroll_NextCmd; Tbl_00970A
 */
void ScrollCmd_CameraB(uint32_t *d0, uint32_t d1, uint32_t *x, uint32_t *y, uint16_t *d6, uint16_t *d7,
                       uint32_t *bands, const u16 **list, uint32_t *layer)
{
    *bands = *(const u32 *)*list;
    *list += 2;
    *x = (uint32_t)((int32_t)(uint32_t)g_ViewX >> 8);
    fill_plane_b(d0, d1, x, y, d6, d7, bands);
}

/* ScrollCmd_CameraOfsB - parallax command $18: as ScrollCmd_CameraB with a 16.16 offset from the
 * list added to the view's x first.
 * In:  list (a5) = a long band table, a long x offset; g_ViewX, g_ViewY
 * Out: as ScrollCmd_CameraB; list past both operands
 * Evidence: add.l (a5)+,d2 before the asr.l #8: the only difference from ScrollCmd_CameraB
 */
void ScrollCmd_CameraOfsB(uint32_t *d0, uint32_t d1, uint32_t *x, uint32_t *y, uint16_t *d6, uint16_t *d7,
                          uint32_t *bands, const u16 **list, uint32_t *layer)
{
    *bands = *(const u32 *)*list;
    uint32_t offset = *(const u32 *)(*list + 2);
    *list += 4;
    *x = (uint32_t)((int32_t)((uint32_t)g_ViewX + offset) >> 8);
    fill_plane_b(d0, d1, x, y, d6, d7, bands);
}
