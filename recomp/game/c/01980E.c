#include "game.h"

/* the boss script after the native's jsr $7FE.w: op $3B16, go to script vector 2 */
static constexpr uint32_t Scr2_01981C = 0x01981Cu;

/* Boss_Harley_BranchAimReached - native fragment of the Harley boss script: when the aim (sprite
 * variable 0) has reached its target (variable 1) the script continues at $1981C (op $3B16 = go to
 * the script vector $3A(a6)), else at $19822 through Boss_Harley_AimNotReached
 * In:  spr = the boss's sprite block (a5); the rest the object's registers (for the script)
 * Out: d0 low word = the target; the script continues at Scr2_01981C or (Boss_Harley_AimNotReached)
 *      $19822
 * Evidence: move.w $24(a5),d0 / cmp.w $22(a5),d0 / bne Boss_Harley_AimNotReached / jsr $7FE; the
 *           script word after the first jsr $7FE is $3B16 (movea.l $3A(a6),a2); TAS 89 frames
 *           f4980-f5075, 88 of them not reached. d0 is an output (both exits hand it on)
 */
void Boss_Harley_BranchAimReached(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                  uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4,
                                  const Vm2Sprite *spr, uint32_t a6)
{
    uint16_t target = spr->var[1];
    *d0 = (*d0 & 0xFFFF0000u) | target;
    if (target != (uint16_t)spr->var[0]) {
        dk::jump(at_Boss_Harley_AimNotReached);
        return;
    }
    dk::jump_script(at_VM2_Enter, Scr2_01981C);
}
