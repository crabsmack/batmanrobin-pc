#include "game.h"

/* Op2_IncVar0 / Op2_DecVar0: script variable 0 (a4 +$22) counted up or down ($002002-$002011). */

/* Op2_IncVar0 - a2 op: adds 1 to script variable 0.
 * In:  vm->var[0]
 * Out: vm->var[0] + 1; NEXT
 * Evidence: addq.w #1,$22(a4) / NEXT; 21 corpus runs incl. the TAS
 */
void Op2_IncVar0(const u16 **script, Vm2Block *vm)
{
    vm->var[0] = (uint16_t)(vm->var[0] + 1);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_DecVar0 - a2 op: subtracts 1 from script variable 0.
 * In:  vm->var[0]
 * Out: vm->var[0] - 1; NEXT
 * Evidence: subq.w #1,$22(a4) / NEXT; 1 corpus run (the TAS)
 */
void Op2_DecVar0(const u16 **script, Vm2Block *vm)
{
    vm->var[0] = (uint16_t)(vm->var[0] - 1);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
