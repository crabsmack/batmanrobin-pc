#include "game.h"

/* Scroll_BandsFromLayer - the band origin from a map layer's pixel position (x as 16.8, y; the
 * Scroll_LayerToScroll arithmetic, x not negated), then falls into Scroll_FillBands.
 * In:  layer (a6), lines = the line buffer (a0), bands = the band table (a1)
 * Out: x (d2) = (((col + 2) * 8) | fine x) << 8; y (d3) = ((row + 2) * 8) | fine y; then as
 *      Scroll_FillBands
 * Evidence: moveq #0,d2 / move.w $32(a6),d2 / addq.w #2 / asl.w #3 / or.b / asl.l #8; the same for
 *           d3 with $34 / +2; no rts, runs into $9928; only caller ScrollCmd_LayerA
 */
void Scroll_BandsFromLayer(uint32_t *d0, uint32_t d1, uint32_t *x, uint32_t *y, uint16_t *d6, uint16_t *fine,
                           u16 *lines, const s16 **bands, const MapLayer *layer)
{
    uint16_t px = (uint16_t)((uint16_t)(layer->col + 2) << 3);
    px |= (uint16_t)(layer->x & 0xF);
    *x = (uint32_t)px << 8;
    uint16_t py = (uint16_t)((uint16_t)(layer->row + 2) << 3);
    *fine = (uint16_t)(layer->y & 0xF);
    *y = (uint16_t)(py | *fine);
    dk::jump(at_Scroll_FillBands);
}
