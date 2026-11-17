#include "game.h"

/* the spotted room's stage header, palette lines 0 and 3, a6 graphics script and trigger table
   (ROM) */
static constexpr uint32_t Tbl_04017A = 0x04017Au;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_03FDE8 = 0x03FDE8u;
static constexpr uint32_t Tbl_0401B6 = 0x0401B6u;
static constexpr uint32_t Tbl_03412E = 0x03412Eu;

enum {
    MUSIC_SPOTTED_ROOM = 0x0A,
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

/* Stage3_RunSpottedRoom - section $3300, the stage 3 red spotted room: music $A, the stage init,
 * palettes 0 and 3, the scene art, the triggers, the room's scroll (Stage3_SpottedRoomInitScroll),
 * the camera locked, both players' health refilled; then every frame the triggers, the objects,
 * the camera following the players' x and the room drawn in perspective by line and column scroll
 * (Stage3_SpottedRoomScroll), until the section's exit (in Frame_End) unwinds it
 * In:  the registers the section runner passes through
 * Out: never returns here: the scene-exit path ends the section
 * Evidence: move.w #$A,d0 / Sound_PlayMusic / lea Tbl_04017A(pc),a6 / Scene_InitStage /
 *           Pal_LoadLine0 Tbl_015634 / Pal_LoadLine3 Tbl_03FDE8 / Tbl_0401B6 VM6_Run /
 *           Trigger_LoadX Tbl_03412E / Stage3_SpottedRoomInitScroll / Camera_LockHere / #$100 into
 *           g_Player1+4 and g_Player2+4; the loop Frame_BeginStage / Trigger_RunX / Obj_RunList /
 *           Camera_TrackPlayersX / Stage3_SpottedRoomScroll / Frame_End; Tbl_00A0AC entry $3300;
 *           TAS f101674 (png f102000: a red room of black holes in perspective)
 */
void Stage3_RunSpottedRoom(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                           uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, MUSIC_SPOTTED_ROOM);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Tbl_04017A;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, a1, &cc);
    a0 = Tbl_03FDE8;
    Pal_LoadLine3(&a0, a1, &cc);
    *a6 = Tbl_0401B6;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    load_triggers(Tbl_03412E, d6, &d7, &a0, a1, a2, a3);
    Stage3_SpottedRoomInitScroll();
    Camera_LockHere();
    ((PlayerRec *)g_Player1)->health = FULL_HEALTH;
    ((PlayerRec *)g_Player2)->health = FULL_HEALTH;
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        PTR(const TriggerKey) next;
        next.v = *a2;
        PTR(const u8) table;
        table.v = *a3;
        const TriggerKey *n = next.get();
        const u8 *t = table.get();
        Trigger_RunX(&d0, d1, &d2, *d3, d4, d5, d6, &d7, &a0, a1, &n, &t, a4, a5, a6);
        *a2 = dk::addr(n);
        *a3 = dk::addr(t);
        Obj_RunList(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Camera_TrackPlayersX(d1, &d2);
        Stage3_SpottedRoomScroll(d1, &d2, d3, d4, d5, d6, a2, a3);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
