#include "game.h"

/* the continue screen's a6 scripts (ROM): the plain-scene header and the graphics */
static constexpr uint32_t Scr6_048EDE = 0x048EDEu;
static constexpr uint32_t Scr6_048F4E = 0x048F4Eu;

enum { FONT_TILE_BASE = 0xE1C9 };   /* the text font's tile base word */

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Continue_Run - the continue screen after a game over (Mr. Freeze's head between two countdown
 * monitors, CREDITS n): plain scene, the graphics, the text-map font, the controller object, then
 * the scene loop without the Start skip (the controller handles Start).
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop; g_SceneResult 1 = continue
 * Evidence: lea Scr6_048EDE(pc),a6 / Scene_InitPlain / Scr6_048F4E VM6_Run / move.w #$E1C9,d0 /
 *           TextMap_LoadFont / bsr.w Continue_SpawnController / bra.w Scene_LoopUnskippable; jsr
 *           from Main_RunSection after music $13; scenario-continue f41970
 */
void Continue_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a2, uint32_t *a3,
                  uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0, a0 = 0, a1 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr6_048EDE);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_048F4E;
    /* the script runs to its end (Op6_End) and leaves its registers to the rest */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a1, a2, a3, a4, a5, a6);
    set_low_word(&d0, FONT_TILE_BASE);
    TextMap_LoadFont(FONT_TILE_BASE, &a0);
    w1 = (uint16_t)*d1;
    Continue_SpawnController(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopUnskippable);
}
