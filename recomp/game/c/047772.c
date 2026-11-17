#include "game.h"

/* the lab section's stage header, palette line 0, line 3, a6 graphics script and trigger table
   (ROM) */
static constexpr uint32_t Tbl_0476EA = 0x0476EAu;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_047520 = 0x047520u;
static constexpr uint32_t Tbl_047726 = 0x047726u;
static constexpr uint32_t Tbl_041000 = 0x041000u;

enum {
    MUSIC_LAB = 0x0B,
    LAB_FLOOR_Y = 0xB8,        /* g_FloorY: the flat floor's world y */
    SCROLL_LINES = 0x0C,       /* g_ScrollMode: per-line H-scroll of both planes */
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

/* Stage4_RunLab - section $4300 of stage 4 (the lab with machines over a tiled floor): music $B,
 * the stage init, the flat floor at y $B8, palettes 0 and 3, the scene art, the triggers, per-line
 * scroll (mode $C); then every frame the stage frame with Stage4_LabScroll, until the section's
 * exit (the scene-exit callback in Frame_End) unwinds it.
 * In:  the registers the section runner passes through
 * Out: never returns here: the scene-exit path ends the section
 * Evidence: move.w #$B,d0 / Sound_PlayMusic / lea Tbl_0476EA(pc),a6 / Scene_InitStage / move.w
 *           #$B8,g_FloorY / Pal_LoadLine0 Tbl_015634 / Pal_LoadLine3 Tbl_047520 / Tbl_047726
 *           VM6_Run / Trigger_LoadX Tbl_041000 / move.w #$C,g_ScrollMode / the Frame_BeginStage /
 *           Frame_RunWorld / Stage4_LabScroll / Frame_End loop; Tbl_00A0AC maps $4300 here; TAS
 *           f119385-122651
 */
void Stage4_RunLab(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                   uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, MUSIC_LAB);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Tbl_0476EA;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    g_FloorY = LAB_FLOOR_Y;
    a0 = Tbl_015634;
    dk::Flags cc{};   /* the condition codes nothing here reads */
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Tbl_047520;
    Pal_LoadLine3(&a0, a1, &cc);
    *a6 = Tbl_047726;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    load_triggers(Tbl_041000, d6, &d7, &a0, a1, a2, a3);
    g_ScrollMode = SCROLL_LINES;
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorld(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        uint16_t w4 = (uint16_t)*d4;
        Stage4_LabScroll(d1, &d2, d3, &w4, d5, d6, a2, a3);
        set_low_word(d4, w4);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
