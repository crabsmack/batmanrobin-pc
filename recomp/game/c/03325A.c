#include "game.h"

/* the restart loaders: the $2200 loader's segments shortened (ROM) */
static constexpr uint32_t Scr6_033120 = 0x033120u;   /* $2300: id, corridor without waves, cloud art, Flight_StartClouds */
static constexpr uint32_t Scr6_033182 = 0x033182u;   /* $2400: id, sky only, hull map, Flight_StartAirship */
static constexpr uint32_t Scr6_0331F0 = 0x0331F0u;   /* $2401: id, sky only, Flight_StartBeamShip */

enum { FLIGHT_MUSIC = 0x09 };

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Stage2_RunFlightClouds - section $2300: restarts the flying section at the cloud bank: music 9,
 * then Flight_RunScene with the loader Scr6_033120 (id $2300, the corridor without waves, the cloud
 * art, Flight_StartClouds)
 * In:  the registers the section runner passes through
 * Out: never returns here (Flight_RunScene)
 * Evidence: move.w #9,d0 / jsr Sound_PlayMusic / pea Scr6_033120(pc) / bra.s Flight_RunScene;
 *           Tbl_00A0AC entry $00A0DE (id $2300); level-skip scenario f5145
 */
void Stage2_RunFlightClouds(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                            uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d7 = 0;   /* not inputs (measured) */
    set_low_word(&d0, FLIGHT_MUSIC);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    dk::push_long(Scr6_033120);      /* pea: Flight_RunScene pops it as its loader script */
    dk::jump(at_Flight_RunScene);    /* bra */
}

/* Stage2_RunFlightAirship - section $2400: restarts the flying section at the airship hull
 * (loader Scr6_033182: id $2400, sky only, the hull map, Flight_StartAirship); the music goes on
 * (no Sound_PlayMusic)
 * In:  the registers the section runner passes through
 * Out: never returns here (Flight_RunScene)
 * Evidence: pea Scr6_033182(pc) / bra.s Flight_RunScene; Tbl_00A0AC entry $00A0E4 (id $2400);
 *           runtime entry in the level-skip scenario
 */
void Stage2_RunFlightAirship(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                             uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    dk::push_long(Scr6_033182);
    dk::jump(at_Flight_RunScene);    /* bra */
}

/* Stage2_RunFlightBoss - section $2401: restarts the flying section at the final beam-ship boss
 * (loader Scr6_0331F0: id $2401, sky only, Flight_StartBeamShip); no Sound_PlayMusic
 * In:  the registers the section runner passes through
 * Out: never returns here (Flight_RunScene)
 * Evidence: pea Scr6_0331F0(pc), then the fall-through into Flight_RunScene (the next file);
 *           Tbl_00A0AC entry $00A0EA (id $2401); runtime entry in the level-skip scenario
 */
void Stage2_RunFlightBoss(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                          uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    dk::push_long(Scr6_0331F0);
    dk::jump(at_Flight_RunScene);    /* falls into it */
}
