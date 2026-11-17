#include "game.h"

/* the ring tunnel section's stage header, palette line 0, line 3, a6 graphics script and trigger
   table (ROM) */
static constexpr uint32_t Tbl_04795A = 0x04795Au;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_047520 = 0x047520u;
static constexpr uint32_t Tbl_047996 = 0x047996u;
static constexpr uint32_t Tbl_041124 = 0x041124u;

enum {
    MUSIC_TUBE = 0x07,
    SCROLL_LINE_PAIRS = 0x10,  /* g_ScrollMode: interleaved plane A/B words per line */
    BLUE_TINT = 2,             /* lines 0 and 3 a little bluer */
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

/* Stage4_RunTube - section $4400 of stage 4 (the jetpack flight through a ring tunnel): music 7,
 * the stage init, palettes 0 and 3, the scene art, the camera locked, the triggers, the view, the
 * interleaved line-pair scroll (mode $10), a blue tint on lines 0 and 3; then every frame the stage
 * frame with Stage4_TubeScroll, until the section's exit unwinds it.
 * In:  the registers the section runner passes through
 * Out: never returns here: the scene-exit path ends the section
 * Evidence: move.w #7,d0 / Sound_PlayMusic / lea Tbl_04795A(pc),a6 / Scene_InitStage / Pal_LoadLine0
 *           / Pal_LoadLine3 / Tbl_047996 VM6_Run / Camera_LockHere / Trigger_LoadX Tbl_041124 /
 *           Camera_UpdateView / move.w #$10,g_ScrollMode / move.b #2,g_Pal0TintB / g_Pal3TintB /
 *           the frame loop; Tbl_00A0AC maps $4400 here; TAS f122651-128749
 */
void Stage4_RunTube(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                    uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, MUSIC_TUBE);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Tbl_04795A;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    a0 = Tbl_015634;
    dk::Flags cc{};   /* the condition codes nothing here reads */
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Tbl_047520;
    Pal_LoadLine3(&a0, a1, &cc);
    *a6 = Tbl_047996;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    Camera_LockHere();
    load_triggers(Tbl_041124, d6, &d7, &a0, a1, a2, a3);
    Camera_UpdateView(&d0, d1, &d2);
    g_ScrollMode = SCROLL_LINE_PAIRS;
    g_Pal0TintB = BLUE_TINT;
    g_Pal3TintB = BLUE_TINT;
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorld(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        uint16_t w4 = (uint16_t)*d4;
        Stage4_TubeScroll(d1, &d2, d3, &w4, d5, d6, a2, a3, a4, a5);
        set_low_word(d4, w4);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
