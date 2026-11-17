#include "game.h"

enum {
    SCENE_VDP_HINT_OFF = 0x8004,       /* reg 0: H-int off */
    SCENE_VDP_AUTOINC_2 = 0x8F02,      /* reg 15: auto-increment 2 */
    SCENE_CLEAR_BYTES = 0x75F8,        /* $FF8000 up to g_VIntCount */
    SCENE_POOL_BLOCKS = 0x8C,          /* 140 object pool blocks at g_ObjPool */
    SCENE_WHITE_LEVEL = 0x0E,          /* g_FadeLevel at full white */
};

/* Scene_InitCommon - resets the machine for a new scene with the V-int held off: clears work RAM
 * from $FF8000, the VDP's H-scroll table, sprite table, CRAM (white after a fade to white, else
 * black), VSRAM and planes, the object lists, pool, boxes, DMA queue and buffer pointers, then
 * applies the scene header: section id, art cache size, VDP registers, the setup routine, the DMA
 * budget, sprite attribute and two mode flags.
 * In:  header (a6) = the SceneHeader; g_FadeLevel
 * Out: header (a6) past it; g_FrameCallback = SceneCb_None, g_SceneResult = 0, the Spr_EmitClipped
 *      limits, g_WaveTimerStep = $100; the callees' registers: d0 = $05FC0000 | VDP_LoadRegs' word
 *      (VDP_ClearPlanes left $05FC05FC, the word moves since keep the high word), d1 / d2 / a2 /
 *      a3 ArtCache_Init's, d6 Mem_Clear's, d7 VDP_LoadRegs', a4 Layer_ResetTileQueue's
 * Evidence: move.b #1,g_VIntOff / st.b g_FrameBusy / $8004 / $8F02 / Mem_Clear $FF8000 $75F8 /
 *           VDP_ClearHScroll / VDP_ClearSAT / cmpi.b #$E,g_FadeLevel / blt VDP_ClearCRAM else
 *           VDP_WhiteCRAM / VDP_ClearVSRAM / VDP_ClearPlanes / the list inits / ObjPool_Init
 *           $FFAD54 $8C / ... / (a6)+ reads / jsr (a0) / move.l #$9CE6,g_FrameCallback; callers
 *           Scene_InitStage, Scene_InitPlain; verify finds every call interrupted (the V-int
 *           lands in the long clears): D2 checks it
 */
void Scene_InitCommon(uint32_t *d0, uint16_t *d1, uint32_t *d2, uint16_t *d6, uint32_t *d7, uint32_t *a2,
                      uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    g_VIntOff = 1;
    g_FrameBusy = 0xFF;
    hw::port_write16(hw::VDP_CTRL, SCENE_VDP_HINT_OFF);
    hw::port_write16(hw::VDP_CTRL, SCENE_VDP_AUTOINC_2);
    uint16_t size = SCENE_CLEAR_BYTES;
    Mem_Clear(d6, &size, (u8 *)&g_ArtHandles);
    VDP_ClearHScroll();
    VDP_ClearSAT();
    if ((int8_t)g_FadeLevel >= SCENE_WHITE_LEVEL)
        VDP_WhiteCRAM();
    else
        VDP_ClearCRAM();
    VDP_ClearVSRAM();
    uint32_t r0 = VDP_ClearPlanes();
    VIntObj_InitList();
    Obj_InitList();
    ObjPool_Init(SCENE_POOL_BLOCKS, ADDR68(g_ObjPool));
    Hitbox_InitLists();
    DMA_ResetQueue();
    Scene_ResetBufPtrs();
    *a4 = ADDR68(Layer_ResetTileQueue());

    PTR(const SceneHeader) at;
    at.v = *a6;
    const SceneHeader *header = at;
    WS_SCENE_BEGIN(*a6);                          /* native, phase 6: the screen's id (R5) */
    uint16_t section = header->section_id;
    if (section != 0)
        g_SectionId = section;
    *d1 = header->art_blocks;
    ArtCache_Init(d1, d2, a2, a3);
    uint16_t cmd = section;                       /* move.w (a6)+,d0: VDP_LoadRegs replaces it */
    VDP_LoadRegs(&cmd, d7, header->vdp_regs);
    *d0 = (r0 & 0xFFFF0000u) | cmd;
    SceneSetupFn(dk::code((uint32_t)header->setup));
    g_DMABudget = header->dma_budget;
    g_SceneSprAttr = header->spr_attr;
    g_SceneShadowHL = header->shadow_hl;
    g_SceneFixedSpr = header->fixed_spr;
    g_WaveTimerStep = 0x100;
    g_SprDirectMaxY = 0x100;
    g_SprDirectBiasY = 0x60;
    g_SprDirectMaxX = 0x160;
    g_SprDirectBiasX = 0x60;
    g_FrameCallback = at_SceneCb_None;
    g_SceneResult = 0;
    *a6 += sizeof(SceneHeader);
}
