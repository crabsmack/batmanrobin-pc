#include "game.h"

/* the street's stage header, palette lines 0 and 2-3, a6 graphics script, fire frame list and
   trigger table (ROM) */
static constexpr uint32_t Tbl_01FA76 = 0x01FA76u;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_01FA36 = 0x01FA36u;
static constexpr uint32_t Tbl_01FAB2 = 0x01FAB2u;
static constexpr uint32_t Tbl_015A7E = 0x015A7Eu;
static constexpr uint32_t Tbl_015F5A = 0x015F5Au;

enum {
    STAGE1_MUSIC_STREET = 0x08,
    STAGE1_SCROLL_LINES = 0x0C,     /* g_ScrollMode: per-line H-scroll of both planes */
    STAGE1_FACADE_BUF = 1,          /* g_BufPtrs[1]: the unpacked facade map (its long header skipped) */
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

/* Stage1_RunStreet - section $1100: the stage 1 street (the bank of Gotham City): music 8, the
 * stage init, palettes, the facade V-int object and the fire frame list, per-line scroll (mode $C)
 * and the street triggers; then every frame the stage frame with Stage1_UpdateStreetScroll and both
 * planes' V-scroll = the view y, until the section's exit (Scene_FadeOutExit) unwinds it
 * In:  the registers the section runner passes through
 * Out: never returns here
 * Evidence: move.w #8,d0 / Sound_PlayMusic / lea Tbl_01FA76(pc),a6 / Scene_InitStage /
 *           Pal_LoadLine0 Tbl_015634 / Pal_LoadLine2 Tbl_01FA36 / Pal_LoadLine3 (the next 32 bytes) /
 *           Tbl_01FAB2 VM6_Run / move.l #$15A7E,g_SceneVars / g_BufPtrs+4 + 4 -> g_SceneVars+$E /
 *           VIntObj_New Stage1_VIntFacade / move.w #$C,g_ScrollMode / Trigger_LoadX Tbl_015F5A / the
 *           Frame_BeginStage / Frame_RunWorld / Stage1_UpdateStreetScroll / g_VScrollA = g_VScrollB =
 *           g_ViewY / Frame_End loop; Tbl_00A0AC maps $1100 here; TAS f503
 */
void Stage1_RunStreet(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                      uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, STAGE1_MUSIC_STREET);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Tbl_01FA76;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Tbl_01FA36;
    Pal_LoadLine2(&a0, a1, &cc);
    Pal_LoadLine3(&a0, a1, &cc);   /* the 16 colours after line 2's */
    *a6 = Tbl_01FAB2;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);

    Stage1StreetVars *vars = (Stage1StreetVars *)g_SceneVars;
    vars->fire_frame = Tbl_015A7E;
    PTR(u8) *bufs = (PTR(u8) *)g_BufPtrs;
    a0 = bufs[STAGE1_FACADE_BUF].raw() + 4;
    vars->map_buffer = a0;
    VIntObj_New(&d0, at_Stage1_VIntFacade, a1, a6);
    g_ScrollMode = STAGE1_SCROLL_LINES;
    load_triggers(Tbl_015F5A, d6, &d7, &a0, a1, a2, a3);
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorld(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        Stage1_UpdateStreetScroll(d1, &d2, d3, d4, d6, *a1);
        uint16_t view_y = (uint16_t)((uint32_t)g_ViewY >> 16);
        set_low_word(&d0, view_y);
        g_VScrollA = view_y;
        g_VScrollB = view_y;
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
