#include "game.h"

/* ScrollCmd_CameraA - parallax command $C: plane A's line H-scroll buffer filled in bands from the
 * view position (x = g_ViewX as 16.8, y = g_ViewY's integer word), g_VScrollA = that y; then the
 * next command.
 * In:  list (a5) = a long band table; g_ViewX, g_ViewY
 * Out: g_HScrollLinesA filled; g_VScrollA; bands (a1) past the pairs used; list past the operand;
 *      x (d2) / y (d3) and the fill's d0/d6/d7
 * Evidence: move.l g_ViewX,d2 / asr.l #8,d2 / move.w g_ViewY,d3 / bsr Scroll_FillBands / move.w
 *           d3,g_VScrollA / bra Scroll_NextCmd; ran in stage 4 (TAS f108331-f113271)
 */
void ScrollCmd_CameraA(uint32_t *d0, uint32_t d1, uint32_t *x, uint32_t *y, uint16_t *d6, uint16_t *d7,
                       uint32_t *bands, const u16 **list, uint32_t *layer)
{
    *bands = *(const u32 *)*list;
    *list += 2;
    *x = (uint32_t)((int32_t)(uint32_t)g_ViewX >> 8);
    *y = (*y & 0xFFFF0000u) | (uint16_t)(g_ViewY >> 16);   /* the word at g_ViewY: the integer part */
    PTR(const s16) b;
    b.v = *bands;
    const s16 *band = b;
    Scroll_FillBands(d0, d1, x, *y, d6, d7, (u16 *)g_HScrollLinesA, &band);
    *bands = dk::addr(band);
    g_VScrollA = (uint16_t)*y;
    dk::jump(at_Scroll_NextCmd);
}
