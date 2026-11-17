#include "game.h"

/* Layer_ScrollStep - per-frame layer scroll: applies up to 16 px of the pending x, then of the
 * pending y, queueing the block column / row each exposes (Layer_ScrollStepX, then
 * Layer_ScrollStepY).
 * In:  layer = the map layer (a6)
 * Out: what Layer_ScrollStepX leaves, then Layer_ScrollStepY's (jumped to)
 * Evidence: bsr.w Layer_ScrollStepX / bra.w Layer_ScrollStepY
 */
void Layer_ScrollStep(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6, uint16_t *d7,
                      uint32_t *a0, uint32_t *a2, uint32_t *a3, MapLayer *layer)
{
    uint32_t d0 = 0;   /* Layer_ScrollStepY doesn't read it */
    Layer_ScrollStepX(&d0, d1, d2, d3, d4, d5, d6, d7, a0, a2, a3, layer);
    dk::jump(at_Layer_ScrollStepY);
}
