#include "game.h"

/* the road section's stage header, palette line 0, a6 graphics script and trigger table (ROM) */
static constexpr uint32_t Tbl_0404E4 = 0x0404E4u;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Tbl_040520 = 0x040520u;
static constexpr uint32_t Scr5_034932 = 0x034932u;

enum {
    MUSIC_HATTER = 0x06,
    ROAD_CAMERA_X = 0xA5,      /* the camera's x (integer part; the road driver moves it after) */
    ROAD_TINT = -2,            /* palette lines 1 and 2 darkened by 2 */
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

/* Stage3_RunHatterRoad - section $3401, the Mad Hatter boss on the checkered road through space:
 * music 6, the stage init, palette line 0, the scene art, the triggers, the camera at x $A5,
 * palette lines 1 and 2 darkened by 2, the road set up (Stage3_InitHatterRoad); then every frame
 * the triggers, the objects and the road-end swirl after them (the road driver object, not
 * Camera_TrackPlayersX, moves the camera), until the section's exit (in Frame_End) unwinds it
 * In:  the registers the section runner passes through
 * Out: never returns here: the scene-exit path ends the section
 * Evidence: move.w #6,d0 / Sound_PlayMusic / lea Tbl_0404E4(pc),a6 / Scene_InitStage /
 *           Pal_LoadLine0 Tbl_015634 / Tbl_040520 VM6_Run / Trigger_LoadX Scr5_034932 / move.w
 *           #$A5,(g_CameraX).w / Camera_UpdateView / moveq #-2,d0 / six move.b d0 into
 *           g_Pal1TintR-B, g_Pal2TintR-B / bsr.w Stage3_InitHatterRoad; the loop Frame_BeginStage /
 *           Trigger_RunX / Obj_RunList / Stage3_RunRoadEndSwirl / Frame_End; Tbl_00A0AC entry
 *           $3401; TAS f113271 (png f113400: the top hat above a bending checkered road)
 */
void Stage3_RunHatterRoad(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                          uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0;   /* not inputs (measured): the callees set them */
    set_low_word(&d0, MUSIC_HATTER);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *a6 = Tbl_0404E4;
    Scene_InitStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, a6);
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, a1, &cc);
    *a6 = Tbl_040520;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    load_triggers(Scr5_034932, d6, &d7, &a0, a1, a2, a3);
    g_CameraX = ((uint32_t)ROAD_CAMERA_X << 16) | (g_CameraX & 0xFFFFu);   /* move.w: the integer part */
    Camera_UpdateView(&d0, d1, &d2);
    d0 = (uint32_t)ROAD_TINT;
    g_Pal1TintR = (uint8_t)d0;
    g_Pal1TintG = (uint8_t)d0;
    g_Pal1TintB = (uint8_t)d0;
    g_Pal2TintR = (uint8_t)d0;
    g_Pal2TintG = (uint8_t)d0;
    g_Pal2TintB = (uint8_t)d0;
    Stage3_InitHatterRoad((uint16_t)*d4, (uint16_t)*d5, a3, a4, *a6);
    for (;;) {
        Frame_BeginStage(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        PTR(const TriggerKey) next;
        next.v = *a2;
        PTR(const u8) table;
        table.v = *a3;
        const TriggerKey *n = next;
        const u8 *t = table;
        Trigger_RunX(&d0, d1, &d2, *d3, d4, d5, d6, &d7, &a0, a1, &n, &t, a4, a5, a6);
        *a2 = dk::addr(n);
        *a3 = dk::addr(t);
        Obj_RunList(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        uint16_t w0 = (uint16_t)d0;
        Stage3_RunRoadEndSwirl(&w0, &a0, a2, a3, a4, a5, a6);
        set_low_word(&d0, w0);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}
