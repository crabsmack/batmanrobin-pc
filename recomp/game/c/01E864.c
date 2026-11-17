#include "game.h"

/* the claw script after the native's jsr $7FE.w: restart coroutines 0/1, the claw leaves */
static constexpr uint32_t Scr2_01E86E = 0x01E86Eu;

/* Boss_Claw_BranchHarleyBeaten - native fragment of the claw script: when g_SceneVars byte 0 is set
 * (Harley's final form beaten, Boss_HarleyJet_SetBeatenFlag) resumes the script at $1E86E (restart
 * coroutines 0/1: the claw leaves), else at $1E87E through Boss_Claw_HarleyNotBeaten
 * In:  the claw's registers (Op2_Native at $1E862)
 * Out: the script continues at Scr2_01E86E or (Boss_Claw_HarleyNotBeaten) $1E87E, every register
 *      as it came
 * Evidence: tst.b g_SceneVars / beq Boss_Claw_HarleyNotBeaten / jsr $7FE.w; cleared by
 *           Boss_Claw_Spawn, set at TAS f12637 by Boss_HarleyJet_SetBeatenFlag; ran f11992-f12638
 *           (508 frames)
 */
void Boss_Claw_BranchHarleyBeaten(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                  uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                                  uint32_t a6)
{
    if (g_SceneVars[0] == 0) {
        dk::jump(at_Boss_Claw_HarleyNotBeaten);
        return;
    }
    dk::jump_script(at_VM2_Enter, Scr2_01E86E);
}
