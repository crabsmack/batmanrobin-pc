#include "game.h"

/* Op2_SetSprVar1 - a2 op: sets sprite variable 1.
 * In:  script = word value
 * Out: spr->var[1]; NEXT
 * Evidence: move.w (a2)+,$24(a5) / NEXT; the same never-run op family follows ($40A0-$4128);
 *           $10512 steps $24(a5); 3 corpus runs; $18DC0 `4098 000C`
 */
void Op2_SetSprVar1(const u16 **script, Vm2Sprite *spr)
{
    spr->var[1] = (int16_t)*(*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
