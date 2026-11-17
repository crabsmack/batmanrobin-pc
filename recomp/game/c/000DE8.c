#include "game.h"

/* the stepping script's a2, which Op2_StepCoroutineN pushed before entering the coroutine */
static const u16 *pop_script(void)
{
    PTR(const u16) caller;
    caller.v = dk::pop_long();
    return caller;
}

/* Op2_SetTimer3Yield3 - a2 op for coroutine 3: timer 3 = the operand, then the coroutine yields
 * (slot 3 resumes at the next op) and the stepping script, its a2 popped from the stack, continues.
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[3], vm->coroutine[3]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$20(a4) / move.l a2,$44(a4) / movea.l (a7)+,a2 / NEXT; executed in the TAS
 */
void Op2_SetTimer3Yield3(const u16 **script, Vm2Block *vm)
{
    vm->timer[3] = *(*script)++;
    vm->coroutine[3] = *script;
    *script = pop_script();
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
