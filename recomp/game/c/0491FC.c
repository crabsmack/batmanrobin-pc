#include "game.h"

/* the controller's script after the jsr $7FE.w: Scene_EndAfterDelay with result 1 */
static constexpr uint32_t Scr2_049204 = 0x049204u;

/* Continue_UseCredit - the Start case of the continue screen's Start check: takes one credit, then
 * the script ends the scene with result 1 (continue) after 30 frames.
 * In:  g_Credits; the controller's registers (the script runs on with them)
 * Out: g_Credits - 1; the script continues at Scr2_049204 (jsr $7FE.w)
 * Evidence: subq.w #1,g_Credits / jsr $7FE.w; case 1 of the op $2D40 switch at $0491F0; ran only
 *           in scenario-continue (f42201)
 */
void Continue_UseCredit(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                        uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    g_Credits -= 1;
    dk::jump_script(at_VM2_Enter, Scr2_049204);
}
