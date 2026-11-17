#include "game.h"

/* the a6 script that loads the sky art once the corridor has scrolled away (ROM) */
static constexpr uint32_t Tbl_0330FC = 0x0330FCu;

enum {
    FLIGHT_SCROLL_LINES = 0x0C,        /* g_ScrollMode: per-line H-scroll of both planes */
    CORRIDOR_GONE_X = 0x680,           /* g_LayerA x when the corridor has left the screen */
    CLOUDS_GONE_X = 0xE40,             /* g_LayerA x when the cloud bank has */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* the cloud bank's layer x (signed compares: cmpi.w / blt) */
static int16_t cloud_layer_x(void) { return (int16_t)((const MapLayer *)g_LayerA)->x; }

/* Flight_FrameCorridorToClouds - frame callback at the corridor's exit: the x-triggers; while the
 * cloud bank's layer x < $680 both the corridor (Flight_UpdateCorridor) and the cloud bank draw
 * (Flight_FrameCorridorClouds); then scroll mode $C, the sky-art loader Tbl_0330FC, and the
 * callback becomes Flight_FrameClouds, which also runs this frame
 * In:  the registers Flight_RunScene's frame passes (Trigger_RunX's next/table in a2/a3)
 * Out: as Trigger_RunX, then Flight_FrameCorridorClouds or Flight_FrameClouds
 * Evidence: jsr Trigger_RunX / cmpi.w #$680,g_LayerA / blt.w Flight_FrameCorridorClouds / move.w
 *           #$C,d0 / Scroll_SetMode / lea Tbl_0330FC(pc),a6 / VM6_Start / move.l
 *           #Flight_FrameClouds,g_SceneVars+$98, then the fall-through; stored by
 *           Flight_StartClouds; TAS f46291-f47955 (1 px a frame: $680 px = 1664 frames)
 */
void Flight_FrameCorridorToClouds(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                  uint32_t *d6, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                                  uint32_t *a5, uint32_t *a6)
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
    if (cloud_layer_x() < CORRIDOR_GONE_X) {
        dk::jump(at_Flight_FrameCorridorClouds);   /* blt.w into the next-but-one file */
        return;
    }
    Scroll_SetMode(FLIGHT_SCROLL_LINES);
    *a6 = Tbl_0330FC;
    VM6_Start(*a6);
    g_FlightScene->frame_fn = at_Flight_FrameClouds;
    uint16_t w4 = (uint16_t)*d4, w6 = (uint16_t)*d6, w7 = (uint16_t)*d7;
    Flight_FrameClouds(d1, d2, d3, &w4, d5, &w6, &w7, a2, a3, a6);
    set_low_word(d4, w4);
    set_low_word(d6, w6);
    set_low_word(d7, w7);
}

/* Flight_FrameClouds - frame callback of the cloud bank: until its layer x reaches $E40 it scrolls
 * the wobbling cloud bank (Flight_ScrollCloudBank) over the sky (Flight_DrawSky); then the bank is
 * flagged gone (releasing Flight_WaitCloudsPassed) and the callback becomes Flight_DrawSky alone
 * In:  the registers Flight_RunScene's frame passes
 * Out: g_SceneVars+$A3 cleared and +$98 = Flight_DrawSky; or Flight_ScrollCloudBank, then
 *      Flight_DrawSky's
 * Evidence: cmpi.w #$E40,g_LayerA / blt.s / sf.b g_SceneVars+$A3 / move.l
 *           #Flight_DrawSky,g_SceneVars+$98 / rts; bsr.w Flight_ScrollCloudBank, then the
 *           fall-through into Flight_DrawSky (the next file); stored by
 *           Flight_FrameCorridorToClouds; TAS f47955-f49938
 */
void Flight_FrameClouds(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6,
                        uint16_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a6)
{
    if (cloud_layer_x() >= CLOUDS_GONE_X) {
        g_FlightScene->clouds_on = 0;
        g_FlightScene->frame_fn = at_Flight_DrawSky;
        return;
    }
    uint32_t a1 = 0;   /* this prototype's clobber */
    Flight_ScrollCloudBank(d1, d2, d3, d4, d5, d6, &a1, a2, a3, a6);
    dk::jump(at_Flight_DrawSky);   /* falls into it */
}
