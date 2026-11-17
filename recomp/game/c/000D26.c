#include "game.h"

/* the stepping script's a2, which Op2_StepCoroutineN pushed before entering the coroutine */
static const u16 *pop_script(void)
{
    PTR(const u16) caller;
    caller.v = dk::pop_long();
    return caller;
}

/* Op2_SetTimer0Yield2 - a2 op for coroutine 2: timer 0 = the operand, then the coroutine yields
 * (slot 2 resumes at the next op) and the stepping script, its a2 popped from the stack, continues.
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[0], vm->coroutine[2]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$1A(a4) / move.l a2,$40(a4) / movea.l (a7)+,a2 / NEXT; executed in the TAS
 */
void Op2_SetTimer0Yield2(const u16 **script, Vm2Block *vm)
{
    vm->timer[0] = *(*script)++;
    vm->coroutine[2] = *script;
    *script = pop_script();
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
