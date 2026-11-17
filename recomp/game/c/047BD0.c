#include "game.h"

/* the Mr. Freeze fight section's stage header, palette line 0, line 3, a6 graphics script,
   animated tiles and trigger table (ROM) */
static constexpr uint32_t Tbl_047B48 = 0x047B48u;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_047520 = 0x047520u;
static constexpr uint32_t Tbl_047B84 = 0x047B84u;
static constexpr uint32_t Tbl_0B29C6 = 0x0B29C6u;
static constexpr uint32_t Tbl_041138 = 0x041138u;

enum {
    MUSIC_FREEZE = 0x0F,
    ANIM_TILE = 0x3C6,         /* the animated tiles' first tile */
    ANIM_PERIOD = 4,           /* a step every 5 frames */
    SKIP_DELAY = 0x1E,         /* g_SceneSkipDelay: Start ignored for 30 frames */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Trigger_LoadX on a ROM table, its typed results back into the registers. The stage tables are
   never empty, so its src / dst (a2 / a3) are written before they are read. */
static void load_triggers(uint32_t table, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2,
                          uint32_t *a3)
{
    PTR(const u8) rom;
    rom.v = table;
    const u8 *t = rom.get();
    TriggerKey *end = nullptr, *src = nullptr, *dst = nullptr;
    uint16_t count = (uint16_t)*d7;
    Trigger_LoadX(d6, &count, &t, &end, &src, &dst);
    set_low_word(d7, count);
    *a0 = dk::addr(t);
    *a1 = dk::addr(end);
    *a2 = dk::addr(src);
    *a3 = dk::addr(dst);
}

/* Stage4_RunFreezeFight - section $4401 of stage 4 (the Mr. Freeze fight in the domed tower
 * room): music $F, the stage init, palettes 0 and 3, the scene art, plane A's animated tiles (a
 * step every 5 frames), the triggers (Boss_Freeze_Spawn), the camera locked here, the cone
 * raster; then every frame the stage frame, the tile animation, the cones, and both planes'
 * V-scroll = g_ViewY, until the section's exit unwinds it.
 * In:  the registers the section runner passes through
 * Out: never returns here: the scene-exit path ends the section
 * Evidence: move.w #$F,d0 / Sound_PlayMusic / lea Tbl_047B48(pc),a6 / Scene_InitStage /
 *           Pal_LoadLine0 / Pal_LoadLine3 / Tbl_047B84 VM6_Run / Layer_SetAnimTiles Tbl_0B29C6 $3C6
 *           / move.w #4,$48(a6) / Trigger_LoadX Tbl_041138 / Camera_LockHere / bsr.w
 *           Stage4_InitConeRaster / the frame loop (Layer_AnimateTiles, Stage4_DrawCones, g_ViewY);
 *           Tbl_00A0AC maps $4401 here; TAS f128749-130772
 */
void Stage4_RunFreezeFight(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                           uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, MUSIC_FREEZE);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Tbl_047B48;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    a0 = Tbl_015634;
    dk::Flags cc{};   /* the condition codes nothing here reads */
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Tbl_047520;
    Pal_LoadLine3(&a0, a1, &cc);
    *a6 = Tbl_047B84;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);

    MapLayer *layer = (MapLayer *)g_LayerA;
    PTR(const s32) anim;   /* the animated tiles' header: two offsets (the frame list, the art) */
    anim.v = Tbl_0B29C6;
    *a6 = dk::addr(layer);
    Layer_SetAnimTiles(ANIM_TILE, anim.get(), layer);
    layer->anim_delay = ANIM_PERIOD;
    load_triggers(Tbl_041138, d6, &d7, &a0, a1, a2, a3);
    Camera_LockHere();
    Stage4_InitConeRaster();
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorld(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        *a6 = dk::addr(layer);
        uint16_t w1 = (uint16_t)*d1, w5 = (uint16_t)*d5;
        Layer_AnimateTiles(&w1, &d2, d3, d4, &w5, a0, a4, a5, layer);
        set_low_word(d1, w1);
        set_low_word(d5, w5);
        Stage4_DrawCones(d1, &d2, d3, d4, d5, d6, a2, a3, a4, a6);
        uint16_t view_y = (uint16_t)((uint32_t)g_ViewY >> 16);
        set_low_word(&d0, view_y);
        g_VScrollA = view_y;
        g_VScrollB = view_y;
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}

/* Scene_LoopSkippableDelayed - the scene loop entry that first holds Start off for 30 frames
 * (g_SceneSkipDelay = $1E), then Scene_LoopSkippable (the next file).
 * In:  the registers the scene routines hand on
 * Out: as Scene_LoopSkippable (never returns here)
 * Evidence: move.w #$1E,g_SceneSkipDelay, falls into $047C6A; callers the attract scenes $049DA6,
 *           $049DEC, $049F62 (bra.w); TAS f10
 */
void Scene_LoopSkippableDelayed(uint32_t d0, uint32_t *d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                uint32_t *d6, uint32_t d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                                uint32_t *a5, uint32_t *a6)
{
    g_SceneSkipDelay = SKIP_DELAY;
    dk::jump(at_Scene_LoopSkippable);
}
