#include "game.h"

/* the GAME OVER scene's a6 scripts (ROM): the plain-scene header and the graphics */
static constexpr uint32_t Scr6_048EF0 = 0x048EF0u;
static constexpr uint32_t Scr6_048F02 = 0x048F02u;

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* GameOver_Run - scene routine of the GAME OVER screen (the falling text over a snowy Gotham
 * skyline): plain scene, the graphics, the director object and the GAME OVER text, then the
 * Start-skippable scene loop.
 * In:  the registers the scene routines pass through
 * Out: ends through the scene loop (g_SceneResult 0 after 1000 frames, 1 on Start)
 * Evidence: lea Scr6_048EF0(pc),a6 / Scene_InitPlain / Scr6_048F02 VM6_Run / bsr.w
 *           GameOver_SpawnDirector / bsr.w GameOver_SpawnText / bra.w Scene_LoopSkippable; only
 *           caller Main_NoContinue; scenario-game-over f42807; png f43200
 */
void GameOver_Run(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2,
                  uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d2 = 0, d7 = 0;   /* not inputs (measured): the callees set them */
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitPlain(&d0, &w1, &d2, &w6, &d7, a2, a3, a4, Scr6_048EF0);
    set_low_word(d1, w1);
    set_low_word(d6, w6);
    *a6 = Scr6_048F02;
    /* the script runs to its end (Op6_End) and leaves its registers to the spawners */
    VM6_Run(&d0, d1, &d2, d3, d4, d5, d6, &d7, a1, a2, a3, a4, a5, a6);
    w1 = (uint16_t)*d1;
    GameOver_SpawnDirector(&w1, (uint16_t)*d4, (uint16_t)*d5, a3, a4, a6);
    GameOver_SpawnText(&d0, &w1, (uint16_t)*d4, (uint16_t)*d5, &d7, a2, a3, a4, *a6);
    set_low_word(d1, w1);
    dk::jump(at_Scene_LoopSkippable);
}
