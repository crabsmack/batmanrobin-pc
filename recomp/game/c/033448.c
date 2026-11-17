#include "game.h"

enum {
    SKY_FAR_STEP = 7,      /* px a frame the far cloud layer drifts left */
    SKY_NEAR_STEP = 3,     /* the near layer */
    SKY_LINE_PAIRS = 0x70, /* Mem_FillLongs count - 1: 112 longs = 224 lines (the two layers alternate) */
};

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Flight_DrawSky - plane B sky of the flying section after the corridor: V-scroll = 7/8 of the
 * view y (three arithmetic halvings summed); H-scroll for all 224 lines from the two drifting
 * cloud layers (-7 and -3 px a frame), alternating line by line, the pair swapped when the frame
 * parity and the V-scroll's low bit differ
 * In:  g_ViewY, g_FrameParity, g_SceneVars+$9C/$9E
 * Out: g_VScrollB; d2's low word = it; d3's low byte = (parity ^ it) >> 1; the drift words; d0 =
 *      the line pair long, a0 = g_HScrollLinesB, d7 = $70, then Mem_FillLongs fills the lines
 * Evidence: move.w g_ViewY,d0 / asr.w #1 / move.w d0,d2 / asr / add / asr / add / move.w
 *           d2,g_VScrollB / move.l g_SceneVars+$9C,d0 / subq.w #3 / swap / subq.w #7 / swap /
 *           move.l / move.b g_FrameParity,d3 / eor.b d2,d3 / lsr.b #1 / bcc / swap / lea
 *           g_HScrollLinesB,a0 / move.w #$70,d7 / jmp Mem_FillLongs; the frame callback
 *           (Flight_StartSkyOnly) and called by Flight_FrameAirship, Flight_FrameBeamShip and
 *           Flight_FrameClouds' tail; TAS f47955-f79303. The prototype gains &d0 (measured a
 *           clobber: no caller reads it) so the jump hands Mem_FillLongs its fill long
 */
void Flight_DrawSky(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d7, uint32_t *a0, uint32_t a1)
{
    int16_t half = (int16_t)(uint16_t)(g_ViewY >> 16) >> 1;
    uint16_t vscroll = (uint16_t)half;
    half >>= 1;
    vscroll = (uint16_t)(vscroll + half);
    half >>= 1;
    vscroll = (uint16_t)(vscroll + half);
    set_low_word(d2, vscroll);
    g_VScrollB = vscroll;

    FlightSceneVars *sky = g_FlightScene;
    sky->sky_near_x = (int16_t)(sky->sky_near_x - SKY_NEAR_STEP);
    sky->sky_far_x = (int16_t)(sky->sky_far_x - SKY_FAR_STEP);
    uint32_t pair = ((uint32_t)(uint16_t)sky->sky_far_x << 16) | (uint16_t)sky->sky_near_x;

    uint8_t odd = (uint8_t)(g_FrameParity ^ (uint8_t)vscroll);
    *d3 = (*d3 & 0xFFFFFF00u) | (uint8_t)(odd >> 1);
    if (odd & 1)
        pair = (pair << 16) | (pair >> 16);   /* swap: the other layer on the even lines */
    *d0 = pair;
    *a0 = dk::addr(g_HScrollLinesB);
    *d7 = SKY_LINE_PAIRS;
    dk::jump(at_Mem_FillLongs);   /* jmp: it returns to our caller */
}

/* Flight_FrameCorridorClouds - the corridor-exit frame's two-layer part: the corridor on plane B
 * (Flight_UpdateCorridor), then on into the cloud bank on plane A (Flight_ScrollCloudBank)
 * In:  the registers Flight_FrameCorridorToClouds hands on
 * Out: as Flight_UpdateCorridor, then Flight_ScrollCloudBank
 * Evidence: blt.w here from Flight_FrameCorridorToClouds while g_LayerA x < $680; jsr
 *           Flight_UpdateCorridor, then the fall-through into Flight_ScrollCloudBank (the next
 *           file); TAS f46291-f47955
 */
void Flight_FrameCorridorClouds(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                uint16_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                                uint32_t *a4, uint32_t *a6)
{
    Flight_UpdateCorridor(d1, d2, d3, d4, d5, d6, d7, a4);
    dk::jump(at_Flight_ScrollCloudBank);   /* falls into it */
}
