#include "game.h"

/* the controller's script after the jsr $7FE.w: the slow countdown and the Start check */
static constexpr uint32_t Scr2_0491D6 = 0x0491D6u;

/* Continue_HasCredits - the credits-left exit of the continue controller's credit test: back into
 * its script at Scr2_0491D6, which starts the slow countdown (coroutine 0: a monitor digit every
 * 60 frames) and the Start check (coroutine 1), then bobs and scrolls the picture every frame.
 * In:  the controller's registers (the script runs on with them)
 * Out: the script continues at Scr2_0491D6 (jsr $7FE.w)
 * Evidence: jsr $7FE.w; the bne target of the tst.w g_Credits at $0491BC; scenario-continue /
 *           scenario-game-over f41999
 */
void Continue_HasCredits(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                         uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    dk::jump_script(at_VM2_Enter, Scr2_0491D6);
}
