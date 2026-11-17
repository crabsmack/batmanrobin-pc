#include "game.h"

/* the boss script after the native's jsr $7FE.w: the aim keeps turning */
static constexpr uint32_t Scr2_019822 = 0x019822u;

/* Boss_Harley_AimNotReached - the not-yet-aimed exit of Boss_Harley_BranchAimReached: resumes the
 * script at $19822
 * In:  the object's registers (for the script)
 * Out: the script continues at Scr2_019822 (jsr $7FE.w) with every register as it came
 * Evidence: jsr $7FE pops $19822; 88 TAS frames f4980-f5074
 */
void Boss_Harley_AimNotReached(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                               uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                               uint32_t a6)
{
    dk::jump_script(at_VM2_Enter, Scr2_019822);
}
