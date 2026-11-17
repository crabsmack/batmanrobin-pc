#include "game.h"

/* Op2_SetSprVar2 - a2 op: sets sprite variable 2.
 * In:  script = word value
 * Out: spr->var[2]; NEXT
 * Evidence: move.w (a2)+,$26(a5) / NEXT; $104BA returns $26(a5) to a native switch; 89 corpus
 *           runs; $4B6A2 `4132 0000`
 */
void Op2_SetSprVar2(const u16 **script, Vm2Sprite *spr)
{
    spr->var[2] = (int16_t)*(*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
