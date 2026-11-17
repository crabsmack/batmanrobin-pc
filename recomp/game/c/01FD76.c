#include "game.h"

/* the warehouse's stage header, palette line 0, a6 graphics script, trigger table and parallax
   list (ROM) */
static constexpr uint32_t Scr6_01FCE6 = 0x01FCE6u;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Scr6_01FD22 = 0x01FD22u;
static constexpr uint32_t Tbl_016632 = 0x016632u;
static constexpr uint32_t Tbl_00972C = 0x00972Cu;

enum {
    STAGE1_MUSIC_WAREHOUSE = 0x01,
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

/* Stage1_RunWarehouse - section $1301: the stage 1 warehouse (crates, the crane-claw boss): music 1,
 * the stage init with Scr6_01FCE6, palette line 0, plane A drawn and flushed by a V-int object, the
 * triggers Tbl_016632, the view; then every frame the stage frame with the layer-A scroll and the
 * parallax list Tbl_00972C, until the section's exit unwinds it
 * In:  the registers the section runner passes through
 * Out: never returns here
 * Evidence: move.w #1,d0 / Sound_PlayMusic / lea Scr6_01FCE6(pc),a6 / Scene_InitStage / Pal_LoadLine0
 *           Tbl_015634 / Scr6_01FD22 VM6_Run / g_LayerA Layer_DrawScreen / VIntObj_New
 *           Layer_FlushTileQueueIfIdle / Trigger_LoadX Tbl_016632 / Camera_UpdateView / the loop. Its
 *           `lea g_LayerA,a5` before Layer_ScrollStep loads a5, not a6 (kept: the step runs on the a6
 *           Frame_RunWorldLayerA left); Tbl_00A0AC maps $1301 here; TAS f9615
 */
void Stage1_RunWarehouse(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                         uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, STAGE1_MUSIC_WAREHOUSE);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Scr6_01FCE6;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, a1, &cc);
    *a6 = Scr6_01FD22;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);

    *a6 = dk::addr(g_LayerA);
    Layer_DrawScreen(&d2, d3, d4, d5, d6, d7, a3, a4, *a5, a6);
    VIntObj_New(&d0, at_Layer_FlushTileQueueIfIdle, a1, a6);
    load_triggers(Tbl_016632, d6, &d7, &a0, a1, a2, a3);
    Camera_UpdateView(&d0, d1, &d2);
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorldLayerA(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        *a5 = dk::addr(g_LayerA);     /* the original's a5 (see above) */
        PTR(MapLayer) layer;
        layer.v = *a6;
        uint16_t w4 = (uint16_t)*d4, w6 = (uint16_t)*d6, w7 = (uint16_t)d7;
        Layer_ScrollStep(d1, &d2, d3, &w4, d5, &w6, &w7, &a0, a2, a3, layer);
        PTR(const u16) at;
        at.v = Tbl_00972C;
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
