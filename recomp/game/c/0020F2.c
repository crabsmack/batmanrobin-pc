#include "game.h"

/* The branch ops on script variable 1 (a4 +$24): a loop counter and the mask / equal tests
 * ($0020F2-$00214F). Each ends at the word branch offset: Op2_Goto takes it, or NEXT after it. */

/* the conditional branch's two exits: Op2_Goto (script at the offset word), or past the offset */
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

/* Op2_DecVar1Loop - a2 op: loop counter on script variable 1: decrements it and branches while
 * it is not zero.
 * In:  script = word branch offset; vm->var[1]
 * Out: vm->var[1] - 1; the branch while nonzero, else NEXT after the offset
 * Evidence: subq.w #1,$24(a4) / bne.w Op2_Goto / addq.l #2,a2 + NEXT; 28 corpus runs incl. the TAS
 */
void Op2_DecVar1Loop(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    vm->var[1] = (uint16_t)(vm->var[1] - 1);
    branch_if(vm->var[1] != 0, handler, script);
}

/* Op2_BranchVar1Any - a2 op: branches when script variable 1 has any of the mask's bits.
 * In:  script = word mask, word branch offset; vm->var[1]
 * Out: value = var1 & mask; the branch, or NEXT after the offset
 * Evidence: move.w $24(a4),d0 / and.w (a2)+,d0 / bne.w Op2_Goto; 151 corpus runs incl. the TAS
 */
void Op2_BranchVar1Any(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *value = (uint16_t)(vm->var[1] & *(*script)++);
    branch_if(*value != 0, handler, script);
}

/* Op2_BranchVar1None - a2 op: branches when script variable 1 has none of the mask's bits.
 * In:  script = word mask, word branch offset; vm->var[1]
 * Out: value = var1 & mask; the branch, or NEXT after the offset
 * Evidence: move.w $24(a4),d0 / and.w (a2)+,d0 / beq.w Op2_Goto; 103 corpus runs incl. the TAS
 */
void Op2_BranchVar1None(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *value = (uint16_t)(vm->var[1] & *(*script)++);
    branch_if(*value == 0, handler, script);
}

/* Op2_BranchVar1Eq - a2 op: branches when script variable 1 equals a word.
 * In:  script = word value, word branch offset; vm->var[1]
 * Out: value = var1; the branch, or NEXT after the offset
 * Evidence: move.w $24(a4),d0 / cmp.w (a2)+,d0 / beq.w Op2_Goto; 5 corpus runs incl. the TAS
 */
void Op2_BranchVar1Eq(uint16_t *value, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *value = vm->var[1];
    branch_if(*value == *(*script)++, handler, script);
}
