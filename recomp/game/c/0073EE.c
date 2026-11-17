#include "game.h"

/* Op6_RedrawLayerA - a6 op: redraws a full screen of the map layer g_LayerA one block column per
 * frame through the tile queue, parking the script until it is done.
 * In:  a6 = script (no operands)
 * Out: a0 = g_LayerA; VM6_StartRedraw (the next file)
 * Evidence: lea g_LayerA,a0 / bra VM6_StartRedraw; 2 uses ($330C4, $331E6 `73EE`, after
 *           Op6_MoveLayerAToCamera); TAS f67239
 */
uint32_t Op6_RedrawLayerA(uint32_t a6)
{
    (void)a6;
    uint32_t layer = ADDR68(g_LayerA);
    dk::jump(at_VM6_StartRedraw);
    return layer;
}
