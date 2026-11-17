#include "game.h"

/* Scroll_SetPlaneA - plane A's scroll words g_HScrollA / g_VScrollA from the map layer g_LayerA's
 * position (Scroll_LayerToScroll).
 * In:  g_LayerA
 * Out: g_HScrollA = x (the H-scroll word), g_VScrollA = y; x / y / fine = Scroll_LayerToScroll's
 *      d0 / d1 / d2; layer = g_LayerA (a6)
 * Evidence: lea g_LayerA,a6 / bsr Scroll_LayerToScroll / move.w d0,g_HScrollA / move.w
 *           d1,g_VScrollA; callers $1FECC, $48C36
 */
void Scroll_SetPlaneA(uint32_t *x, uint32_t *y, uint16_t *fine, uint32_t *layer)
{
    const MapLayer *l = (const MapLayer *)g_LayerA;
    *layer = dk::addr(l);
    Scroll_LayerToScroll(x, y, fine, l);
    g_HScrollA = (uint16_t)*x;
    g_VScrollA = (uint16_t)*y;
}
