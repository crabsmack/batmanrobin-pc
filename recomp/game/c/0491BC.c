#include "game.h"

/* the controller's script after the native's no-credits jsr $7FE.w: the fast countdown and the
   NO CREDITS REMAINING text */
static constexpr uint32_t Scr2_0491C6 = 0x0491C6u;

/* Continue_BranchHasCredits - native of the continue controller: with credits left the script
 * goes on at Continue_HasCredits' part (the countdown with Start), with none at Scr2_0491C6.
 * In:  g_Credits; the controller's registers (the script runs on with them)
 * Out: the script continues (jsr $7FE.w) at Scr2_0491C6, or Continue_HasCredits runs
 * Evidence: tst.w g_Credits / bne.s Continue_HasCredits / jsr $7FE.w; entered by Op2_Native at
 *           $0491BA; the corpus always had credits here (Scr2_0491C6 never read);
 *           scenario-continue / scenario-game-over f41999
 */
void Continue_BranchHasCredits(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                               uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    if (g_Credits != 0) {
        dk::jump(at_Continue_HasCredits);
        return;
    }
    dk::jump_script(at_VM2_Enter, Scr2_0491C6);
}
