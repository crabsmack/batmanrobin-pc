#include "game.h"

/* the $2200 loader: four a6 scripts in a row (section ids, corridor art, Flight_StartCorridor; the
   later segments started from wave scripts) (ROM) */
static constexpr uint32_t Tbl_03301E = 0x03301Eu;

enum { FLIGHT_MUSIC = 0x09 };

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Stage2_RunFlight - section $2200: starts stage 2's flying section from its beginning: music 9,
 * then Flight_RunScene with the loader script Tbl_03301E (section id $2200, the corridor art,
 * Flight_StartCorridor) pushed for it
 * In:  the registers the section runner passes through
 * Out: never returns here (the scene ends through Scene_FadeOutExit)
 * Evidence: move.w #9,d0 / jsr Sound_PlayMusic / pea Tbl_03301E(pc) / bra.s Flight_RunScene;
 *           Tbl_00A0AC entry $00A0D8 (id $2200); TAS f35416, right after the lift boss
 */
void Stage2_RunFlight(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1,
                      uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d7 = 0;   /* not inputs (measured) */
    set_low_word(&d0, FLIGHT_MUSIC);
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    dk::push_long(Tbl_03301E);       /* pea: Flight_RunScene pops it as its loader script */
    dk::jump(at_Flight_RunScene);    /* bra */
}
