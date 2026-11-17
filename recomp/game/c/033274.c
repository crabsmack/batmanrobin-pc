#include "game.h"

/* the flying section's stage header, palette lines 0 and 3, and the corridor's x-trigger table
   (one wave) (ROM) */
static constexpr uint32_t Tbl_032FE2 = 0x032FE2u;
static constexpr uint32_t Tbl_015634 = 0x015634u;
static constexpr uint32_t Scr6_032DCE = 0x032DCEu;
static constexpr uint32_t Tbl_021D26 = 0x021D26u;

enum {
    FLIGHT_FULL_HEALTH = 0x100,
    FLIGHT_SCROLL_STEP = 0x180,       /* g_MapScrollX: 1.5 px a frame (16.16) */
    FLIGHT_SCROLL_CORRIDOR = 0x34,    /* g_ScrollMode: plane B lines + the H-int V-scroll table */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Trigger_LoadX on a ROM table, its typed results back into the registers */
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

/* Flight_RunScene - the flying section's scene: the stage init (Tbl_032FE2), the V-int tile-queue
 * flush, palette lines 0 and 3, the airship path object, then the loader script its section
 * routine pushed, the camera locked and both players' health refilled; then every frame: every
 * object and the camera (or the debug free camera), g_MapScrollX += 1.5 px and the part's frame
 * callback, until the section's exit (in Frame_End) unwinds it
 * In:  (sp) = the a6 loader script pushed by the Stage2_RunFlight* entries; a5 (the only register
 *      read before it is written, measured)
 * Out: never returns (the scene ends through Scene_FadeOutExit popping the loop)
 * Evidence: lea Tbl_032FE2(pc),a6 / Scene_InitStage / VIntObj_New Layer_FlushTileQueueIfIdle /
 *           Pal_LoadLine0 Tbl_015634 / Pal_LoadLine3 Scr6_032DCE / jsr Flight_SpawnAirshipPathObj /
 *           movea.l (a7)+,a6 / VM6_Run / Camera_LockHere / move.w #$100,g_Player1+4 / g_Player2+4;
 *           the loop Frame_BeginStage / tst.b g_FreeCam / Debug_FreeCamStep or Obj_RunList /
 *           Camera_TrackPlayersY / Camera_TrackPlayersX, then addi.l #$180,g_MapScrollX / movea.l
 *           g_SceneVars+$98,a0 / jsr (a0) / Frame_End; TAS f35416 until f79303
 */
void Flight_RunScene(uint32_t a5)
{
    /* every register but a5 is written before it is read (measured) */
    uint32_t d0 = 0, d1 = 0, d2 = 0, d3 = 0, d4 = 0, d5 = 0, d6 = 0, d7 = 0;
    uint32_t a0 = 0, a1 = 0, a2 = 0, a3 = 0, a4 = 0, a6 = Tbl_032FE2;
    Scene_InitStage(&d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7, &a2, &a3, &a4, &a6);
    VIntObj_New(&d0, at_Layer_FlushTileQueueIfIdle, &a1, &a6);
    dk::Flags cc{};   /* the condition codes nothing here reads */
    a0 = Tbl_015634;
    Pal_LoadLine0(&a0, &a1, &cc);
    a0 = Scr6_032DCE;
    Pal_LoadLine3(&a0, &a1, &cc);
    uint16_t w1 = (uint16_t)d1;
    Flight_SpawnAirshipPathObj(&d0, &w1, (uint16_t)d4, (uint16_t)d5, &a2, &a3, &a4, &a5, a6);
    set_low_word(&d1, w1);
    a6 = dk::pop_long();   /* movea.l (a7)+,a6: the section's loader script */
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7, &a1, &a2, &a3, &a4, &a5, &a6);
    Camera_LockHere();
    ((PlayerRec *)g_Player1)->health = FLIGHT_FULL_HEALTH;
    ((PlayerRec *)g_Player2)->health = FLIGHT_FULL_HEALTH;
    for (;;) {
        Frame_BeginStage(&d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7, &a0, &a1, &a2, &a3, &a4, &a5, &a6);
        if (g_FreeCam) {
            Debug_FreeCamStep(&d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7, &a0, &a1, &a2, a3, a4, a5, &a6);
        } else {
            Obj_RunList(&d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7, &a0, &a1, &a2, &a3, &a4, &a5, &a6);
            Camera_TrackPlayersY(&d2, &d3, &d4, &d5, &d6, &d7);
            Camera_TrackPlayersX(&d1, &d2);
        }
        g_MapScrollX = g_MapScrollX + FLIGHT_SCROLL_STEP;
        a0 = g_FlightScene->frame_fn;
        FlightFrameFn(a0, &d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7, &a0, &a1, &a2, &a3, &a4, &a5, &a6);
        /* the section's exit runs in here and never comes back */
        Frame_End(&d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7, &a2, &a3, &a4, a5, a6);
    }
}

/* Flight_StartCorridor - loader-script native of the flying section's start: the corridor's
 * x-trigger table Tbl_021D26 (one wave), scroll mode $34, the plane B V-scroll H-int, and the
 * frame callback Flight_FrameCorridor
 * In:  -
 * Out: d0 = 0 and Z set (Op6_CallNativeRetry: done); Trigger_LoadX's d6/d7/a0-a3, then
 *      HInt_InstallVScrollB's d7/a0/a1
 * Evidence: lea Tbl_021D26,a0 / Trigger_LoadX / move.w #$34,d0 / Scroll_SetMode /
 *           HInt_InstallVScrollB / move.l #Flight_FrameCorridor,g_SceneVars+$98 / moveq #0,d0;
 *           `70BE 0003 32FA` at $3304A in the $2200 loader Tbl_03301E; TAS f35416-f35433
 */
void Flight_StartCorridor(uint32_t *d0, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3)
{
    load_triggers(Tbl_021D26, d6, d7, a0, a1, a2, a3);
    set_low_word(d0, FLIGHT_SCROLL_CORRIDOR);
    Scroll_SetMode((uint16_t)*d0);
    HInt_InstallVScrollB(d7, a0, a1);
    g_FlightScene->frame_fn = at_Flight_FrameCorridor;
    *d0 = 0;
}
