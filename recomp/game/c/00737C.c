#include "game.h"

/* Op6_MoveLayerAToCamera - a6 op: places the map layer g_LayerA at the camera position plus an
 * offset (Layer_MoveX / Layer_MoveY: positions only, nothing drawn), then continues.
 * In:  a6 = script: word dx, word dy (pixels, added to g_CameraX / g_CameraY)
 * Out: d0-d2 as Layer_MoveY leaves them; g_A6ScriptPc after the operands, a6 = g_LayerA; then
 *      VM6_Reload
 * Evidence: move.w (g_CameraX).w,d0 / move.w (g_CameraY).w,d1 / add.w (a6)+ twice /
 *           move.l a6,(g_A6ScriptPc).w / lea g_LayerA,a6 / move.w d1,-(a7) / jsr Layer_MoveX /
 *           move.w (a7)+,d0 / jsr Layer_MoveY / bra VM6_Reload; 2 uses: $330BE, $331E0
 *           `737C FEAC 002C`, each followed by Op6_RedrawLayerA; TAS f67239
 */
void Op6_MoveLayerAToCamera(uint16_t *d0, uint16_t *d1, uint16_t *d2, uint32_t *a6)
{
    PTR(const u16) pc;
    pc.v = *a6;
    const u16 *script = pc;
    *d0 = (uint16_t)(g_CameraX >> 16);
    *d1 = (uint16_t)(g_CameraY >> 16);
    *d0 = (uint16_t)(*d0 + vm6_word(&script));
    *d1 = (uint16_t)(*d1 + vm6_word(&script));
    g_A6ScriptPc = ADDR68(script);
    MapLayer *layer = (MapLayer *)g_LayerA;
    *a6 = ADDR68(layer);
    uint16_t dy = *d1;   /* kept on the stack across Layer_MoveX in the original */
    Layer_MoveX(*d0, layer);
    *d0 = dy;
    Layer_MoveY(d0, d1, d2, layer);
    dk::jump(at_VM6_Reload);
}
