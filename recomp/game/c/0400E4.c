#include "game.h"

/* the forest's stage header, palette lines 0 and 3, a6 graphics script, trigger table and parallax
   bands (ROM) */
static constexpr uint32_t Scr6_040018 = 0x040018u;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_03FDE8 = 0x03FDE8u;
static constexpr uint32_t Scr6_040054 = 0x040054u;
static constexpr uint32_t Tbl_033F30 = 0x033F30u;
static constexpr uint32_t Tbl_00976C = 0x00976Cu;

enum {
    MUSIC_STAGE3 = 0x0E,
    FULL_HEALTH = 0x100,
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Trigger_LoadX on a ROM table, its typed results back into the registers (the stage tables are
   never empty, so its src / dst are written before they are read) */
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

/* Stage3_RunMushroomForest - section $3200, the stage 3 mushroom forest: music $E, the
 * stage init, palettes 0 and 3, the scene art, plane A drawn, the V-int tile-queue flush object,
 * the triggers, both players' health refilled; then every frame the world with layer A, the layer
 * scroll and the parallax bands Tbl_00976C, until the section's exit (in Frame_End) unwinds it
 * In:  the registers the section runner passes through
 * Out: never returns here: the scene-exit path ends the section
 * Evidence: move.w #$E,d0 / Sound_PlayMusic / lea Scr6_040018(pc),a6 / Scene_InitStage /
 *           Pal_LoadLine0 Tbl_015634 / Pal_LoadLine3 Tbl_03FDE8 / Scr6_040054 VM6_Run / g_LayerA
 *           Layer_DrawScreen / Layer_FlushTileQueueIfIdle VIntObj_New / Trigger_LoadX Tbl_033F30 /
 *           Camera_UpdateView / move.w #$100,g_Player1+4 / g_Player2+4; the loop Frame_BeginStage /
 *           Frame_RunWorldLayerA / Layer_ScrollStep / Scroll_RunParallax Tbl_00976C / Frame_End;
 *           Tbl_00A0AC entry $3200; TAS f90392 (png f090600: giant mushrooms and trees)
 */
void Stage3_RunMushroomForest(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                       uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, MUSIC_STAGE3);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Scr6_040018;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Tbl_03FDE8;
    Pal_LoadLine3(&a0, a1, &cc);
    *a6 = Scr6_040054;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    MapLayer *layer = (MapLayer *)g_LayerA;
    *a6 = dk::addr(layer);
    Layer_DrawScreen(&d2, d3, d4, d5, d6, d7, a3, a4, *a5, a6);
    a0 = at_Layer_FlushTileQueueIfIdle;
    VIntObj_New(&d0, a0, a1, a6);
    load_triggers(Tbl_033F30, d6, &d7, &a0, a1, a2, a3);
    Camera_UpdateView(&d0, d1, &d2);
    ((PlayerRec *)g_Player1)->health = FULL_HEALTH;
    ((PlayerRec *)g_Player2)->health = FULL_HEALTH;
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorldLayerA(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        *a6 = dk::addr(layer);
        uint16_t w4 = (uint16_t)*d4, w6 = (uint16_t)*d6, w7 = (uint16_t)d7;
        Layer_ScrollStep(d1, &d2, d3, &w4, d5, &w6, &w7, &a0, a2, a3, layer);
        PTR(const u16) bands;
        bands.v = Tbl_00976C;
        const u16 *list = bands.get();
        Scroll_RunParallax(&d0, *d1, &d2, d3, &w6, &w7, a1, &list, a6);
        *a5 = dk::addr(list);
        set_low_word(d4, w4);
        set_low_word(d6, w6);
        set_low_word(&d7, w7);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
