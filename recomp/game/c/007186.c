#include "game.h"

/* VM6_LoadPalettes - a6 helper: finds a palette entry in a scene buffer and copies count
 * palettes from it into the CRAM lines a mask byte picks two bits at a time (Pal_LoadLine0-3).
 * In:  a6 = script: word g_BufPtrs offset, word entry offset (its first word is skipped), byte
 *      count-1, byte mask (bits 7-6 the first palette's line, 5-4 the second's, ...)
 * Out: a6 past the 6 operand bytes; d0's low byte = the mask shifted out, d7 = $0000FFFF (dbra);
 *      a0 past the last palette, a1 as the last Pal_LoadLine leaves it; the condition codes of
 *      its last move.l
 * Evidence: entry + 2 / moveq #0,d7 / move.b (a6)+,d7 / move.b (a6)+,d0 / add.b d0,d0 / bcs
 *           pairs pick Pal_LoadLine0/1/2/3 / dbra d7; bsr from Op6_LoadPalettesYield, the fall-in
 *           of Op6_LoadPalettes; `7182 0000 0004 0040` = 1 palette to line 1; 32 corpus runs
 */
void VM6_LoadPalettes(uint8_t *mask, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script)
{
    uint32_t buf = vm6_slot(vm6_word(script))->raw();
    *a0 = vm6_entry(buf, vm6_word(script)) + 2;
    const u8 *bytes = (const u8 *)*script;
    *d7 = bytes[0];
    *mask = bytes[1];
    *script += 1;
    dk::Flags flags{};
    do {
        int line = *mask >> 6;   /* add.b d0,d0 twice: the carries are bit 7, then bit 6 */
        flags.x = (*mask & 0x40) != 0;
        *mask = (uint8_t)(*mask << 2);
        if (line == 0)
            Pal_LoadLine0(a0, a1, &flags);
        else if (line == 1)
            Pal_LoadLine1(a0, a1, &flags);
        else if (line == 2)
            Pal_LoadLine2(a0, a1, &flags);
        else
            Pal_LoadLine3(a0, a1, &flags);
        *d7 = (*d7 & 0xFFFF0000u) | (uint16_t)(*d7 - 1);
    } while ((uint16_t)*d7 != 0xFFFF);
}

/* Op6_InitLayerAOnPlaneA - a6 op: initialises the map layer g_LayerA from a level header in a scene
 * buffer, drawn on plane A.
 * In:  a6 = script: word g_BufPtrs offset (the level header), word tile word added to every cell
 * Out: g_LayerA.plane_vram = g_PlaneAVRAM, a1 = g_LayerA; VM6_InitLayer (the next file)
 * Evidence: lea g_LayerA,a1 / move.w (g_PlaneAVRAM).w,$24(a1) / bra VM6_InitLayer; 9 uses, e.g.
 *           $1FC30 `71CC 0000 03BA`; every stage scene start
 */
void Op6_InitLayerAOnPlaneA(uint16_t *d0, uint16_t *d1, uint32_t *d2, uint16_t *d6, uint32_t *a1, uint32_t *a2,
                            uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d2, (void)d6, (void)a2, (void)a3, (void)a4, (void)a6;
    MapLayer *layer = (MapLayer *)g_LayerA;
    layer->plane_vram = g_PlaneAVRAM;
    *a1 = ADDR68(layer);
    dk::jump(at_VM6_InitLayer);
}

/* Op6_InitLayerAOnPlaneB - a6 op: initialises the map layer g_LayerA from a level header in a scene
 * buffer, drawn on plane B.
 * In:  a6 = script: word g_BufPtrs offset (the level header), word tile word added to every cell
 * Out: g_LayerA.plane_vram = g_PlaneBVRAM, a1 = g_LayerA; VM6_InitLayer (the next file)
 * Evidence: lea g_LayerA,a1 / move.w (g_PlaneBVRAM).w,$24(a1) / bra VM6_InitLayer; 1 use: $32EB0
 *           `71DA 0000 84A0` (the vertical zeppelin stage); 3 corpus runs
 */
void Op6_InitLayerAOnPlaneB(uint16_t *d0, uint16_t *d1, uint32_t *d2, uint16_t *d6, uint32_t *a1, uint32_t *a2,
                            uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d2, (void)d6, (void)a2, (void)a3, (void)a4, (void)a6;
    MapLayer *layer = (MapLayer *)g_LayerA;
    layer->plane_vram = g_PlaneBVRAM;
    *a1 = ADDR68(layer);
    dk::jump(at_VM6_InitLayer);
}
