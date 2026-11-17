#include "game.h"

/* the Gem Expo's stage header, palette lines 0 and 2-3, a6 graphics script, trigger table and
   parallax list (ROM) */
static constexpr uint32_t Scr6_01FBCA = 0x01FBCAu;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_01FA36 = 0x01FA36u;
static constexpr uint32_t Scr6_01FC06 = 0x01FC06u;
static constexpr uint32_t Tbl_0163BA = 0x0163BAu;
static constexpr uint32_t Tbl_00970A = 0x00970Au;

enum {
    STAGE1_MUSIC_GEM_EXPO = 0x08,
    STAGE1_FULL_HEALTH = 0x100,
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

/* Stage1_RunGemExpo - section $1300: the stage 1 Gem Expo interior: music 8, the stage init with
 * the a6 scene script Scr6_01FBCA, palettes, plane A drawn and flushed by a V-int object, the
 * triggers Tbl_0163BA, both players' health refilled; then every frame the stage frame with the
 * layer-A scroll and the parallax list Tbl_00970A, until the section's exit unwinds it
 * In:  the registers the section runner passes through
 * Out: never returns here
 * Evidence: move.w #8,d0 / Sound_PlayMusic / lea Scr6_01FBCA(pc),a6 / Scene_InitStage / Pal_LoadLine0
 *           Tbl_015634 / Pal_LoadLine2 Tbl_01FA36 / Pal_LoadLine3 / Scr6_01FC06 VM6_Run / g_LayerA
 *           Layer_DrawScreen / VIntObj_New Layer_FlushTileQueueIfIdle / Trigger_LoadX Tbl_0163BA /
 *           move.w #$100,g_Player1+4 / g_Player2+4 / the Frame_BeginStage / Frame_RunWorldLayerA /
 *           Layer_ScrollStep / Scroll_RunParallax Tbl_00970A / Frame_End loop; Tbl_00A0AC maps $1300
 *           here; TAS f6334; shot f7200
 */
void Stage1_RunGemExpo(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                       uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, STAGE1_MUSIC_GEM_EXPO);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Scr6_01FBCA;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Tbl_01FA36;
    Pal_LoadLine2(&a0, a1, &cc);
    Pal_LoadLine3(&a0, a1, &cc);   /* the 16 colours after line 2's */
    *a6 = Scr6_01FC06;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);

    MapLayer *layer = (MapLayer *)g_LayerA;
    *a6 = dk::addr(layer);
    Layer_DrawScreen(&d2, d3, d4, d5, d6, d7, a3, a4, *a5, a6);
    VIntObj_New(&d0, at_Layer_FlushTileQueueIfIdle, a1, a6);
    load_triggers(Tbl_0163BA, d6, &d7, &a0, a1, a2, a3);
    ((PlayerRec *)g_Player1)->health = STAGE1_FULL_HEALTH;
    ((PlayerRec *)g_Player2)->health = STAGE1_FULL_HEALTH;
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorldLayerA(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        *a6 = dk::addr(layer);
        uint16_t w4 = (uint16_t)*d4, w6 = (uint16_t)*d6, w7 = (uint16_t)d7;
        Layer_ScrollStep(d1, &d2, d3, &w4, d5, &w6, &w7, &a0, a2, a3, layer);
        PTR(const u16) at;
        at.v = Tbl_00970A;
        const u16 *list = at;
        Scroll_RunParallax(&d0, *d1, &d2, d3, &w6, &w7, a1, &list, a6);
        *a5 = dk::addr(list);
        set_low_word(d4, w4);
        set_low_word(d6, w6);
        set_low_word(&d7, w7);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
