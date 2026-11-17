#include "game.h"

/* Boss_Freeze_GetDamageStage - the damage stage (0-8) of Mr. Freeze's hover platform, script
 * variable 0, which the platform animation's switch turns into one of nine frames
 * In:  vm = the boss's VM block (a4: +$22 variable 0)
 * Out: d0.w = variable 0
 * Evidence: move.w $22(a4),d0 / rts; only caller Op2_CallNativeRelSwitch at $046598 (nine
 *           Op2_DrawSpr cases $4844-$4894) in the block-$10 animation $046524; Scr2_046176 increments
 *           var 0 at energy 96/92/88/83/79/75/70 %; TAS 128791-130772 (every frame of the fight)
 */
uint16_t Boss_Freeze_GetDamageStage(const Vm2Block *vm)
{
    return vm->var[0];
}
