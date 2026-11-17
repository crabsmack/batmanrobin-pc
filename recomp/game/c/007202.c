#include "game.h"

/* VM6_InitLayer - shared tail of the a6 layer-init ops: Layer_Init on the layer a1 with the level
 * header from a g_BufPtrs slot and the script's tile word, then continues.
 * In:  a1 = the layer (its plane VRAM base already set); a6 = script: word g_BufPtrs offset (the
 *      level header), word tile word
 * Out: d0 = the tile word (as Layer_Init leaves it); g_A6ScriptPc after the operands, a6 = the
 *      layer; then VM6_Reload
 * Evidence: lea g_BufPtrs,a0 / adda.w (a6)+,a0 / movea.l (a0),a0 / move.w (a6)+,d0 /
 *           move.l a6,(g_A6ScriptPc).w / movea.l a1,a6 / jsr Layer_Init / bra VM6_Reload; bra from
 *           Op6_InitLayerAOnPlaneA/B (and the raw $71E8/$71F6); 21 corpus runs
 */
void VM6_InitLayer(uint16_t *d0, uint16_t *d1, uint32_t *d2, uint16_t *d6, uint32_t *a1, uint32_t *a2,
                   uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    PTR(const u16) pc;
    pc.v = *a6;
    const u16 *script = pc;
    PTR(const s32) header;
    header.v = vm6_slot(vm6_word(&script))->raw();
    *d0 = vm6_word(&script);
    g_A6ScriptPc = ADDR68(script);
    *a6 = *a1;
    PTR(MapLayer) layer;
    layer.v = *a6;
    Layer_Init(d0, d1, d2, d6, header, a1, a2, a3, a4, layer);
    dk::jump(at_VM6_Reload);
}
