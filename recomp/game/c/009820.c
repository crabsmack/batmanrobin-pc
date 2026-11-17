#include "game.h"

/* ScrollCmd_LayerA - parallax command 4: plane A's line H-scroll buffer filled in bands from a map
 * layer's position (Scroll_BandsFromLayer), g_VScrollA = the layer's y; then the next command.
 * In:  list (a5) = a long band table, a long map layer
 * Out: g_HScrollLinesA filled; g_VScrollA; bands (a1, past the pairs used) / layer (a6) = the
 *      operands; list past them; the fill's d0/d2/d3/d6/d7
 * Evidence: lea g_HScrollLinesA,a0 / movea.l (a5)+,a1 / movea.l (a5)+,a6 / bsr Scroll_BandsFromLayer
 *           / move.w d3,g_VScrollA / bra Scroll_NextCmd; Tbl_00970A: table $971C, g_LayerA
 */
void ScrollCmd_LayerA(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7,
                      uint32_t *bands, const u16 **list, uint32_t *layer)
{
    const u32 *ops = (const u32 *)*list;          /* the operands: two longs */
    *bands = ops[0];
    *layer = ops[1];
    *list += 4;
    PTR(const s16) b;
    PTR(const MapLayer) l;
    b.v = *bands;
    l.v = *layer;
    const s16 *band = b;
    Scroll_BandsFromLayer(d0, d1, d2, d3, d6, d7, (u16 *)g_HScrollLinesA, &band, l);
    *bands = dk::addr(band);
    g_VScrollA = (uint16_t)*d3;
    dk::jump(at_Scroll_NextCmd);
}
