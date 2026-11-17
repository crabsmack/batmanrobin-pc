#include "game.h"

/* Scroll_LayerToScroll - a map layer's plane scroll words: x = -(((col + 2) * 8) | fine x), y =
 * ((row + 2) * 8) | fine y (the fine offsets are the low 4 bits of the layer's pixel position).
 * In:  layer = the map layer (a6)
 * Out: x = the H-scroll word, its upper word 0; y = the V-scroll word, upper word 0; fine = the
 *      fine y (d2's low word)
 * Evidence: moveq #0,d0 / move.w $32(a6),d0 / addq.w #2 / lsl.w #3 / or.b (fine & $F) / neg.w;
 *           the same for $34 / +2 into d1; only caller Scroll_SetPlaneA (a6 = g_LayerA)
 */
void Scroll_LayerToScroll(uint32_t *x, uint32_t *y, uint16_t *fine, const MapLayer *layer)
{
    uint16_t px = (uint16_t)((uint16_t)(layer->col + 2) << 3);
    px |= (uint16_t)(layer->x & 0xF);
    *x = (uint16_t)(0u - px);                /* neg.w: the upper word stays moveq's 0 */
    uint16_t py = (uint16_t)((uint16_t)(layer->row + 2) << 3);
    *fine = (uint16_t)(layer->y & 0xF);
    *y = (uint16_t)(py | *fine);
}
