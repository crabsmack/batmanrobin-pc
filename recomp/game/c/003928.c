#include "game.h"

/* Op2_BranchBoxStruck and Op2_BranchBoxNotStruck ($003928-$003943). */

/* Op2_BranchBoxStruck - a2 op: branches when the object's body box number i was struck
 * (Obj_TestBoxStruck: g_StruckBox = it).
 * In:  script = word box index, word offset
 * Out: d0/a1 as Obj_TestBoxStruck leaves them; struck: Op2_Goto, else NEXT after the offset
 * Evidence: bsr.w Obj_TestBoxStruck / bne.w Op2_Goto / addq.l #2,a2 / NEXT; 3 runs
 */
void Op2_BranchBoxStruck(uint32_t *d0, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm)
{
    Obj_TestBoxStruck(d0, a1, script, vm);
    if (*d0 != 0) {   /* moveq #1 / moveq #0: Z = (d0 == 0) */
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}

/* Op2_BranchBoxNotStruck - a2 op: branches when the object's body box number i was not struck.
 * In:  script = word box index, word offset
 * Out: d0/a1 as Obj_TestBoxStruck leaves them; not struck: Op2_Goto, else NEXT after the offset
 * Evidence: bsr.w Obj_TestBoxStruck / beq.w Op2_Goto / addq.l #2,a2 / NEXT; 5 runs
 */
void Op2_BranchBoxNotStruck(uint32_t *d0, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm)
{
    Obj_TestBoxStruck(d0, a1, script, vm);
    if (*d0 == 0) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}
