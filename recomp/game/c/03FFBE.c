#include "game.h"

/* the cat-head arena's stage header, palette line 0, a6 graphics script and trigger table (ROM) */
static constexpr uint32_t Scr6_03FF2A = 0x03FF2Au;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Scr6_03FF66 = 0x03FF66u;
static constexpr uint32_t Tbl_033F20 = 0x033F20u;

enum {
    MUSIC_BOSS = 0x04,
    CATHEAD_FLOOR_Y = 0xD0,    /* g_FloorY: the arena's flat floor */
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

/* Stage3_RunCatHeadBoss - section $3101, the stage 3 cat-head boss arena: music 4, the stage init,
 * the flat floor at y $D0, palette line 0, the scene art, the triggers, the camera locked where it
 * is; then every frame the world and Boss_CatHead_FrameScroll, until the section's exit (in
 * Frame_End) unwinds it
 * In:  the registers the section runner passes through
 * Out: never returns here: the scene-exit path ends the section
 * Evidence: move.w #4,d0 / Sound_PlayMusic / lea Scr6_03FF2A(pc),a6 / Scene_InitStage / move.w
 *           #$D0,g_FloorY / Pal_LoadLine0 Tbl_015634 / Scr6_03FF66 VM6_Run / Trigger_LoadX
 *           Tbl_033F20 / Camera_LockHere; the loop Frame_BeginStage / Frame_RunWorld /
 *           Boss_CatHead_FrameScroll / Frame_End; Tbl_00A0AC entry $3101; TAS f87254 (png f087600:
 *           a giant grinning cat face, BOSS TIME)
 */
void Stage3_RunCatHeadBoss(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                           uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, MUSIC_BOSS);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Scr6_03FF2A;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    g_FloorY = CATHEAD_FLOOR_Y;
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, a1, &cc);
    *a6 = Scr6_03FF66;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    load_triggers(Tbl_033F20, d6, &d7, &a0, a1, a2, a3);
    Camera_LockHere();
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Frame_RunWorld(d0, d1, &d2, d3, d4, d5, d6, d7, &a0, a1, a2, a3, a4, a5, a6);
        Boss_CatHead_FrameScroll();
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
