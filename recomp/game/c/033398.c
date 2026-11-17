#include "game.h"

/* the airship part's scroll command list (plane A's line scroll) (ROM) */
static constexpr uint32_t Tbl_0333F2 = 0x0333F2u;

enum { FLIGHT_SCROLL_LINES = 0x0C };   /* g_ScrollMode: per-line H-scroll of both planes */

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Flight_StartAirship - loader-script native of the airship part: the path setup
 * (Flight_AirshipStartPath: the trigger table Tbl_0307C2, the hull map offset), scroll mode $C and
 * the frame callback Flight_FrameAirship
 * In:  -
 * Out: d0 = 0 and Z set (Op6_CallNativeRetry: done); Flight_AirshipStartPath's registers
 * Evidence: jsr Flight_AirshipStartPath(pc) / move.w #$C,d0 / Scroll_SetMode / move.l
 *           #Flight_FrameAirship,g_SceneVars+$98 / moveq #0,d0; `70BE 0003 3398` at $330C6
 *           (Tbl_03301E) and $331E8 (Scr6_033182); TAS f67262
 */
void Flight_StartAirship(uint32_t *d0, uint16_t *d1, uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1,
                         uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    Flight_AirshipStartPath(d1, d6, d7, a0, a1, a2, a3, a4, a6);
    set_low_word(d0, FLIGHT_SCROLL_LINES);
    Scroll_SetMode((uint16_t)*d0);
    g_FlightScene->frame_fn = at_Flight_FrameAirship;
    *d0 = 0;
}

/* Flight_FrameAirship - frame callback of the airship part: the sky (Flight_DrawSky), then plane
 * A's hull map follows the camera plus the map offset (pending scroll = offset + view - layer
 * position; Layer_ScrollStep, Layer_AnimateTiles), and the scroll command list Tbl_0333F2 sets its
 * line scroll
 * In:  the registers Flight_RunScene's frame passes; g_FlightAirship map_x/map_y, g_ViewX/Y
 * Out: g_LayerA pending_x/y and its scroll; then Scroll_NextCmd on Tbl_0333F2 (a5) with a6 =
 *      g_LayerA
 * Evidence: bsr Flight_DrawSky / lea g_LayerA,a6 / move.w g_SceneVars+$94,d0 / add.w g_ViewX,d0 /
 *           sub.w (a6),d0 / move.w d0,4(a6) / the same for y into 6(a6) / Layer_ScrollStep /
 *           Layer_AnimateTiles / lea Tbl_0333F2,a5 / jmp Scroll_NextCmd; stored by
 *           Flight_StartAirship; TAS f67262-f77184
 */
void Flight_FrameAirship(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6,
                         uint16_t *d7, uint32_t a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                         uint32_t *a6)
{
    uint32_t d0 = 0, a0 = 0;   /* this prototype's clobbers */
    Flight_DrawSky(&d0, *d1, d2, d3, d7, &a0, a1);
    MapLayer *layer = (MapLayer *)g_LayerA;
    uint16_t view_x = (uint16_t)(g_ViewX >> 16), view_y = (uint16_t)(g_ViewY >> 16);   /* the integer words */
    layer->pending_x = (int16_t)(uint16_t)(g_FlightAirship->map_x + view_x - layer->x);
    layer->pending_y = (int16_t)(uint16_t)(g_FlightAirship->map_y + view_y - layer->y);
    uint16_t w4 = (uint16_t)*d4;
    Layer_ScrollStep(d1, d2, d3, &w4, d5, d6, d7, &a0, a2, a3, layer);
    set_low_word(d4, w4);
    uint16_t w1 = (uint16_t)*d1, w5 = (uint16_t)*d5;
    Layer_AnimateTiles(&w1, d2, d3, d4, &w5, a0, a4, a5, layer);
    set_low_word(d1, w1);
    set_low_word(d5, w5);
    *a5 = Tbl_0333F2;
    *a6 = dk::addr(layer);
    dk::jump(at_Scroll_NextCmd);   /* jmp: it returns to Flight_RunScene */
}
