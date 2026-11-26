#include "game.h"

/* taken: Op2_Goto reads the offset; not taken: the offset skipped, NEXT (0020F2.c's branch_if) */
static void branch_if(bool taken, uint32_t *handler, const u16 **script)
{
    if (taken) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_BranchVar1Ne - a2 op: branches when script variable 1 differs from a word.
 * In:  script = word value, word branch offset; vm->var[1]
 * Out: value = var1; the branch, or NEXT after the offset
 * Evidence: move.w $24(a4),d0 / cmp.w (a2)+,d0 / bne.w Op2_Goto, Op2_BranchVar1Eq with bne; never run in the corpus.
 *           Prototype: Op2_BranchVar1Eq's (the same code)
 */
void Op2_BranchVar1Ne(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *value = vm->var[1];
    branch_if(*value != *(*script)++, handler, script);
}
