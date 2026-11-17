#include "game.h"

/* Op2_BranchVar1Lt - a2 op: branches when script variable 1 is less than a word (signed).
 * In:  script = word value, word branch offset; vm->var[1]
 * Out: value = var1; the branch (Op2_Goto), or NEXT after the offset
 * Evidence: move.w $24(a4),d0 / cmp.w (a2)+,d0 / blt.w Op2_Goto / addq.l #2,a2 + NEXT; 24 corpus
 *           runs incl. the TAS
 */
void Op2_BranchVar1Lt(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *value = vm->var[1];
    if ((int16_t)*value < (int16_t)*(*script)++) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
