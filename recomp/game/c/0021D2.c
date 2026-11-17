#include "game.h"

/* Op2_BranchVar2Any - a2 op: branches when script variable 2 (a4 +$26) has any of the mask's bits.
 * In:  script = word mask, word branch offset; vm->var[2]
 * Out: value = var2 & mask; the branch (Op2_Goto), or NEXT after the offset
 * Evidence: move.w $26(a4),d0 / and.w (a2)+,d0 / bne.w Op2_Goto / addq.l #2,a2 + NEXT; 19 corpus
 *           runs incl. the TAS
 */
void Op2_BranchVar2Any(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *value = (uint16_t)(vm->var[2] & *(*script)++);
    if (*value != 0) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
