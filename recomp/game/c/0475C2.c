#include "game.h"

/* the hall section's stage header, palette line 0, line 3, a6 graphics script, trigger table and
   parallax command list (ROM) */
static constexpr uint32_t Tbl_047540 = 0x047540u;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_047520 = 0x047520u;
static constexpr uint32_t Tbl_04757C = 0x04757Cu;
static constexpr uint32_t Tbl_040F4A = 0x040F4Au;
static constexpr uint32_t Tbl_0097AA = 0x0097AAu;

enum {
    MUSIC_HALL = 0x0B,
    HALL_FLOOR_Y = 0xC8,       /* g_FloorY: the flat floor's world y */
    HALL_TINT = -2,            /* palette lines 1 and 2: every channel 2 steps darker */
    HALL_FULL_HEALTH = 0x100,  /* both players' records: full health on entering stage 4 */
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

/* Stage4_RunHall - section $4000, stage 4's first scene (the hall with arched windows and a rippling
 * blue floor): music $B, the stage init, the flat floor at y $C8, palettes 0 and 3, the scene art,
 * the triggers, the view, the cone raster, palette lines 1/2 tinted -2, both players' health
 * refilled; then every frame the stage frame, the parallax command list Tbl_0097AA, one cone and
 * the floor ripple, until the section's exit unwinds it.
 * In:  the registers the section runner passes through
 * Out: never returns here: the scene-exit path ends the section
 * Evidence: move.w #$B,d0 / Sound_PlayMusic / lea Tbl_047540(pc),a6 / Scene_InitStage / move.w
 *           #$C8,g_FloorY / Pal_LoadLine0 Tbl_015634 / Pal_LoadLine3 Tbl_047520 / Tbl_04757C VM6_Run
 *           / Trigger_LoadX Tbl_040F4A / Camera_UpdateView / bsr.w Stage4_InitConeRaster / six
 *           move.b #$FE to g_Pal1Tint* / g_Pal2Tint* / move.w #$100 to g_Player1+4 / g_Player2+4 /
 *           the loop Frame_BeginStage / Frame_RunWorld / lea Tbl_0097AA,a5 / Scroll_NextCmd /
 *           Stage4_DrawCones / Stage4_HallFloorRipple / Frame_End; Tbl_00A0AC maps $4000 here; TAS
 *           f116171-119385
 */
void Stage4_RunHall(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                    uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, MUSIC_HALL);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Tbl_047540;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    g_FloorY = HALL_FLOOR_Y;
    a0 = Tbl_015634;
    dk::Flags cc{};   /* the condition codes nothing here reads */
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Tbl_047520;
    Pal_LoadLine3(&a0, a1, &cc);
    *a6 = Tbl_04757C;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    load_triggers(Tbl_040F4A, d6, &d7, &a0, a1, a2, a3);
    Camera_UpdateView(&d0, d1, &d2);
    Stage4_InitConeRaster();
    g_Pal1TintR = (uint8_t)HALL_TINT;
    g_Pal1TintG = (uint8_t)HALL_TINT;
    g_Pal1TintB = (uint8_t)HALL_TINT;
    g_Pal2TintR = (uint8_t)HALL_TINT;
    g_Pal2TintG = (uint8_t)HALL_TINT;
    g_Pal2TintB = (uint8_t)HALL_TINT;
    ((PlayerRec *)g_Player1)->health = HALL_FULL_HEALTH;
    ((PlayerRec *)g_Player2)->health = HALL_FULL_HEALTH;
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorld(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        PTR(const u16) cmds;
        cmds.v = Tbl_0097AA;
        const u16 *list = cmds.get();
        uint16_t w6 = (uint16_t)*d6, w7 = (uint16_t)d7;
        Scroll_NextCmd(&d0, *d1, &d2, d3, &w6, &w7, a1, &list, a6);
        set_low_word(d6, w6);
        set_low_word(&d7, w7);
        *a5 = dk::addr(list);
        Stage4_DrawCones(d1, &d2, d3, d4, d5, d6, a2, a3, a4, a6);
        uint16_t w3 = (uint16_t)*d3, w4 = (uint16_t)*d4, w5 = (uint16_t)*d5;
        Stage4_HallFloorRipple(d1, &d2, &w3, &w4, &w5);
        set_low_word(d3, w3);
        set_low_word(d4, w4);
        set_low_word(d5, w5);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
