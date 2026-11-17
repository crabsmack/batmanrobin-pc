#include "game.h"

enum { CLOUD_BANK_STEP_X = 1 };   /* px a frame the cloud bank scrolls right */

/* Flight_ScrollCloudBank - plane A cloud bank of the flying section: g_LayerA scrolls 1 px right
 * a frame and follows the view y (Layer_ScrollStep), then the wobbling line scroll
 * (Flight_CloudBankLines)
 * In:  the registers the frame callbacks pass; g_ViewY
 * Out: g_LayerA pending_x = 1, pending_y = view y - layer y, then as Layer_ScrollStep (a6 =
 *      g_LayerA) and Flight_CloudBankLines
 * Evidence: lea g_LayerA,a6 / move.w #1,4(a6) / move.w g_ViewY,d0 / sub.w 2(a6),d0 / move.w
 *           d0,6(a6) / jsr Layer_ScrollStep / bra.w Flight_CloudBankLines; called by
 *           Flight_FrameCorridorClouds (fall-through) and Flight_FrameClouds; TAS f46291-f49938
 */
void Flight_ScrollCloudBank(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6,
                            uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a6)
{
    MapLayer *layer = (MapLayer *)g_LayerA;
    *a6 = dk::addr(layer);
    layer->pending_x = CLOUD_BANK_STEP_X;
    layer->pending_y = (int16_t)(uint16_t)((uint16_t)(g_ViewY >> 16) - layer->y);
    uint16_t d7 = 0;      /* this prototype's clobbers: Layer_ScrollStep's d7/a0 */
    uint32_t a0 = 0;
    Layer_ScrollStep(d1, d2, d3, d4, d5, d6, &d7, &a0, a2, a3, layer);
    dk::jump(at_Flight_CloudBankLines);   /* bra.w into the 0334D2 file */
}
