#include "game.h"

/* the cloud bank's x-trigger table: a weapon drop and a wave (ROM) */
static constexpr uint32_t Scr5_021D3A = 0x021D3Au;

enum { FLIGHT_SCROLL_CLOUDS = 0x38 };   /* g_ScrollMode: both planes' lines + the H-int V-scroll */

/* Flight_FrameCorridor - frame callback of the corridor part: the x-triggers, then on to
 * Flight_UpdateCorridor (the perspective corridor on plane B)
 * In:  the registers Flight_RunScene's frame passes (Trigger_RunX's next/table in a2/a3)
 * Out: as Trigger_RunX, then Flight_UpdateCorridor
 * Evidence: jsr Trigger_RunX / jmp Flight_UpdateCorridor; stored in g_SceneVars+$98 by
 *           Flight_StartCorridor(NoWaves), called by Flight_RunScene every frame; TAS f35433-f46290
 */
void Flight_FrameCorridor(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                          uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, a0 = 0, a1 = 0;   /* this prototype's clobbers */
    PTR(const TriggerKey) next;
    PTR(const u8) table;
    next.v = *a2;
    table.v = *a3;
    const TriggerKey *n = next;
    const u8 *t = table;
    Trigger_RunX(&d0, d1, d2, *d3, d4, d5, d6, d7, &a0, &a1, &n, &t, a4, a5, a6);
    *a2 = dk::addr(n);
    *a3 = dk::addr(t);
    dk::jump(at_Flight_UpdateCorridor);   /* jmp: it returns to Flight_RunScene */
}

/* Flight_StartClouds - loader-script native at the corridor's end: the x-trigger table Scr5_021D3A
 * (a weapon drop and a wave), scroll mode $38, the frame callback Flight_FrameCorridorToClouds,
 * the cloud bank's wobble table, and the cloud bank flagged on screen
 * In:  -
 * Out: d0 = 0 and Z set (Op6_CallNativeRetry: done); Trigger_LoadX's d6/a2/a3, then
 *      Flight_BuildWobbleTable's d7/a0/a1
 * Evidence: lea Scr5_021D3A,a0 / Trigger_LoadX / move.w #$38,g_ScrollMode / move.l
 *           #Flight_FrameCorridorToClouds,g_SceneVars+$98 / bsr Flight_BuildWobbleTable / st.b
 *           g_SceneVars+$A3 / moveq #0,d0; `70BE 0003 3356` at $33082 (Tbl_03301E) and $3317A
 *           (Scr6_033120); TAS f46290
 */
void Flight_StartClouds(uint32_t *d0, uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3)
{
    PTR(const u8) rom;
    rom.v = Scr5_021D3A;
    const u8 *t = rom.get();
    TriggerKey *end = nullptr, *src = nullptr, *dst = nullptr;   /* the table is not empty: all written */
    Trigger_LoadX(d6, d7, &t, &end, &src, &dst);
    *a0 = dk::addr(t);
    *a1 = dk::addr(end);
    *a2 = dk::addr(src);
    *a3 = dk::addr(dst);
    g_ScrollMode = FLIGHT_SCROLL_CLOUDS;
    g_FlightScene->frame_fn = at_Flight_FrameCorridorToClouds;
    Flight_BuildWobbleTable(d7, a0, a1);
    g_FlightScene->clouds_on = 0xFF;
    *d0 = 0;
}

/* Flight_WaitCloudsPassed - Op6_CallNativeRetry wait: the loader script waits while the cloud
 * bank is still on screen (Flight_FrameClouds clears the flag at layer x $E40)
 * In:  g_SceneVars+$A3
 * Out: d0's low byte = the flag, Z = it is clear (Op6_CallNativeRetry: nonzero = retry next frame)
 * Evidence: move.b g_SceneVars+$A3,d0 / rts; `70BE 0003 337C` at $33092, right after the section
 *           id $2400 op in Tbl_03301E; TAS f46290-f49938
 */
uint8_t Flight_WaitCloudsPassed(void)
{
    return g_FlightScene->clouds_on;
}
