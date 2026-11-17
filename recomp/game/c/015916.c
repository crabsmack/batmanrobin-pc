#include "game.h"

static constexpr int16_t FREECAM_MAX_STEP = 4;   /* the camera follows the cursor at most 4 px a frame */

static uint16_t camera_x(void) { return (uint16_t)(g_CameraX >> 16); }

/* Debug_FreeCamScroll - tail of Debug_FreeCamStep: scrolls the camera right by up to 4 px toward
 * the cursor (or the lock x), resyncs layer A and updates the view
 * In:  d0 low word = pixels the cursor is right of the screen centre (> 0)
 * Out: g_CameraX's integer part advanced (the whole long cleared if it went negative); then as
 *      Debug_FreeCamSyncLayer
 * Evidence: cmpi.w #4,d0 / blt / move.w #4,d0 / add.w d0,g_CameraX / bpl Debug_FreeCamSyncLayer /
 *           clr.l g_CameraX and the fall into it; static only (the debug free camera). d0 an output
 *           (measured as a clobber): Debug_FreeCamSyncLayer, fallen into, passes it on to
 *           Camera_UpdateView
 */
void Debug_FreeCamScroll(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *a6)
{
    int16_t step = (int16_t)*d0;
    if (step >= FREECAM_MAX_STEP)
        step = FREECAM_MAX_STEP;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)step;
    uint16_t x = (uint16_t)(camera_x() + step);
    g_CameraX = ((uint32_t)x << 16) | (g_CameraX & 0xFFFFu);
    if ((int16_t)x < 0)
        g_CameraX = 0;
    Debug_FreeCamSyncLayer(d0, d1, d2, a6);
}

/* Debug_FreeCamSyncLayer - tail of Debug_FreeCamScroll and the free camera's no-scroll exit: plane
 * A's pending x = camera x - its x, then Camera_UpdateView
 * In:  g_CameraX, g_LayerA
 * Out: a6 = g_LayerA, its pending x = d0 low word = g_CameraX - its x; then as Camera_UpdateView
 * Evidence: lea g_LayerA,a6 / move.w g_CameraX,d0 / sub.w (a6),d0 / move.w d0,4(a6) / jmp
 *           Camera_UpdateView; bra.s from Debug_FreeCamStep (cursor left of centre) and the fall
 *           from Debug_FreeCamScroll; static only
 */
void Debug_FreeCamSyncLayer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *a6)
{
    MapLayer *layer = (MapLayer *)g_LayerA;
    *a6 = ADDR68(layer);
    uint16_t pending = (uint16_t)(camera_x() - layer->x);
    *d0 = (*d0 & 0xFFFF0000u) | pending;
    layer->pending_x = (int16_t)pending;
    dk::jump(at_Camera_UpdateView);
}
