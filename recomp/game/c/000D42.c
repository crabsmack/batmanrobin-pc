#include "game.h"

/* the stepping script's a2, which Op2_StepCoroutineN pushed before entering the coroutine */
static const u16 *pop_script(void)
{
    PTR(const u16) caller;
    caller.v = dk::pop_long();
    return caller;
}

/* Coroutine 2's set-timer-and-yield ops for timers 2 and 3 ($000D42-$000DE7). */

/* the yield: slot 2 resumes at the next op; the stepping script's a2, pushed by
   Op2_StepCoroutine2, comes back off the stack and its NEXT runs */
static void yield2(const u16 **script, Vm2Block *vm)
{
    vm->coroutine[2] = *script;
    *script = pop_script();
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SetTimer2Yield2 - a2 op for coroutine 2: timer 2 = the operand, then the coroutine yields.
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[2], vm->coroutine[2]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$1E(a4) / move.l a2,$40(a4) / movea.l (a7)+,a2 / NEXT; executed in the TAS
 */
void Op2_SetTimer2Yield2(const u16 **script, Vm2Block *vm)
{
    vm->timer[2] = *(*script)++;
    yield2(script, vm);
}

/* Op2_SetTimer3Yield2 - a2 op for coroutine 2: timer 3 = the operand, then the coroutine yields.
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[3], vm->coroutine[2]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$20(a4) / move.l a2,$40(a4) / movea.l (a7)+,a2 / NEXT
 */
void Op2_SetTimer3Yield2(const u16 **script, Vm2Block *vm)
{
    vm->timer[3] = *(*script)++;
    yield2(script, vm);
}
