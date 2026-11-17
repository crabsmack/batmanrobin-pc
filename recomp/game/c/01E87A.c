#include "game.h"

/* the claw script after the native's jsr $7FE.w: its timer-driven attack loop */
static constexpr uint32_t Scr2_01E87E = 0x01E87Eu;

/* Boss_Claw_HarleyNotBeaten - the not-yet-beaten exit of Boss_Claw_BranchHarleyBeaten: resumes the
 * claw script at $1E87E (its timer-driven attack loop)
 * In:  the claw's registers (for the script)
 * Out: the script continues at Scr2_01E87E (jsr $7FE.w) with every register as it came
 * Evidence: beq target of Boss_Claw_BranchHarleyBeaten; Scr2_01E87E starts with
 *           Op2_TickTimer3Branch; TAS f11992-f12636 (509 frames)
 */
void Boss_Claw_HarleyNotBeaten(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                               uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                               uint32_t a6)
{
    dk::jump_script(at_VM2_Enter, Scr2_01E87E);
}
