#include "game.h"

/* the controller's script after the jsr $7FE.w: random shakes and white flashes, looped */
static constexpr uint32_t Scr2_0493DC = 0x0493DCu;

enum { SLOW_FADE_DELAY = 9 };   /* g_FadeDelay: frames between fade steps minus 1 */

/* Continue_SlowWhiteFade - after the continue countdown ran out and Scene_EndAfterDelayWhite
 * started the white fade: makes the fade slow (a step every 10 frames), then the script loops
 * random shakes and white flashes until the scene ends.
 * In:  the controller's registers (coroutine 0 of Scr2_0491A0)
 * Out: g_FadeDelay = 9; the script continues at Scr2_0493DC (jsr $7FE.w)
 * Evidence: move.b #9,g_FadeDelay / jsr $7FE.w; Op2_Native at $493D0; ran only in
 *           scenario-game-over at f42783
 */
void Continue_SlowWhiteFade(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                            uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    g_FadeDelay = SLOW_FADE_DELAY;
    dk::jump_script(at_VM2_Enter, Scr2_0493DC);
}
