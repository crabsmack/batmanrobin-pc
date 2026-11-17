#include "game.h"

/* Op2_StepCoroutine3 ($002432-$00243B). */

/* Op2_StepCoroutine3 - a2 op: runs coroutine slot 3 for one step: pushes the current script and
 * continues at the slot's (back through the return op $2D2A, or a SetTimer3..Yield3 / WaitTimer3
 * op that saves its position, which pops it).
 * In:  vm->coroutine[3]
 * Out: (sp) = the stepping script; script = the slot's; NEXT into its first op
 * Evidence: move.l a2,-(a7) / movea.l $44(a4),a2 / NEXT; dispatched in 57 runs
 */
void Op2_StepCoroutine3(const u16 **script, Vm2Block *vm)
{
    dk::push_long(dk::addr(*script));
    *script = vm->coroutine[3];
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
