#include "game.h"

/* the stepping script's a2, which Op2_StepCoroutineN pushed before entering the coroutine */
static const u16 *pop_script(void)
{
    PTR(const u16) caller;
    caller.v = dk::pop_long();
    return caller;
}

/* Op2_SetTimer0RandYield0 - a2 op for coroutine 0: timer 0 = base + a random value below range
 * (Rand_Below), then the coroutine yields (slot 0 resumes at the next op) and the stepping
 * script, its a2 popped from the stack, continues.
 * In:  script = words range, base; the stepping script's a2 on the stack
 * Out: vm->timer[0], vm->coroutine[0]; rand's low word = the ticks (its upper word Rand_Below's);
 *      script = the popped one, NEXT
 * Evidence: move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 / move.w d0,$1A(a4) /
 *           move.l a2,$38(a4) / movea.l (a7)+,a2 / NEXT; executed in the TAS
 */
void Op2_SetTimer0RandYield0(uint32_t *rand, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *rand = (*rand & 0xFFFF0000u) | *(*script)++;
    Rand_Below(rand);
    *rand = (*rand & 0xFFFF0000u) | (uint16_t)(*rand + *(*script)++);
    vm->timer[0] = (uint16_t)*rand;
    vm->coroutine[0] = *script;
    *script = pop_script();
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
