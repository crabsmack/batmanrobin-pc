#include "game.h"

/* the stepping script's a2, which Op2_StepCoroutineN pushed before entering the coroutine */
static const u16 *pop_script(void)
{
    PTR(const u16) caller;
    caller.v = dk::pop_long();
    return caller;
}

/* Coroutine 1's set-timer-and-yield ops ($000C8E-$000CF5): each sets one timer (to the operand,
 * or base + random below range), stores the next op in slot 1 and yields: the stepping script's
 * a2, pushed by Op2_StepCoroutine1, comes back off the stack and its NEXT runs. */

/* the yield: slot 1 resumes at the next op; the stepping script continues (NEXT) */
static void yield1(const u16 **script, Vm2Block *vm)
{
    vm->coroutine[1] = *script;
    *script = pop_script();
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* base + random(range): move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 (d0's upper word as
   Rand_Below leaves it) */
static uint16_t rand_ticks(uint32_t *rand, const u16 **script)
{
    *rand = (*rand & 0xFFFF0000u) | *(*script)++;
    Rand_Below(rand);
    *rand = (*rand & 0xFFFF0000u) | (uint16_t)(*rand + *(*script)++);
    return (uint16_t)*rand;
}

/* Op2_SetTimer0Yield1 - a2 op for coroutine 1: timer 0 = the operand, then the coroutine yields.
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[0], vm->coroutine[1]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$1A(a4) / move.l a2,$3C(a4) / movea.l (a7)+,a2 / NEXT
 */
void Op2_SetTimer0Yield1(const u16 **script, Vm2Block *vm)
{
    vm->timer[0] = *(*script)++;
    yield1(script, vm);
}

/* Op2_SetTimer1Yield1 - a2 op for coroutine 1: timer 1 = the operand, then the coroutine yields.
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[1], vm->coroutine[1]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$1C(a4) / move.l a2,$3C(a4) / movea.l (a7)+,a2 / NEXT
 */
void Op2_SetTimer1Yield1(const u16 **script, Vm2Block *vm)
{
    vm->timer[1] = *(*script)++;
    yield1(script, vm);
}

/* Op2_SetTimer2Yield1 - a2 op for coroutine 1: timer 2 = the operand, then the coroutine yields.
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[2], vm->coroutine[1]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$1E(a4) / move.l a2,$3C(a4) / movea.l (a7)+,a2 / NEXT
 */
void Op2_SetTimer2Yield1(const u16 **script, Vm2Block *vm)
{
    vm->timer[2] = *(*script)++;
    yield1(script, vm);
}

/* Op2_SetTimer3Yield1 - a2 op for coroutine 1: timer 3 = the operand, then the coroutine yields.
 * In:  script = word ticks; the stepping script's a2 on the stack
 * Out: vm->timer[3], vm->coroutine[1]; script = the popped one, NEXT
 * Evidence: move.w (a2)+,$20(a4) / move.l a2,$3C(a4) / movea.l (a7)+,a2 / NEXT
 */
void Op2_SetTimer3Yield1(const u16 **script, Vm2Block *vm)
{
    vm->timer[3] = *(*script)++;
    yield1(script, vm);
}

/* Op2_SetTimer0RandYield1 - a2 op for coroutine 1: timer 0 = base + random below range, then
 * the coroutine yields.
 * In:  script = words range, base; the stepping script's a2 on the stack
 * Out: vm->timer[0], vm->coroutine[1]; rand's low word = the ticks; script = the popped one, NEXT
 * Evidence: move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 / move.w d0,$1A(a4) / move.l a2,$3C(a4)
 */
void Op2_SetTimer0RandYield1(uint32_t *rand, const u16 **script, Vm2Block *vm)
{
    vm->timer[0] = rand_ticks(rand, script);
    yield1(script, vm);
}

/* Op2_SetTimer1RandYield1 - a2 op for coroutine 1: timer 1 = base + random below range, then
 * the coroutine yields.
 * In:  script = words range, base; the stepping script's a2 on the stack
 * Out: vm->timer[1], vm->coroutine[1]; rand's low word = the ticks; script = the popped one, NEXT
 * Evidence: move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 / move.w d0,$1C(a4) / move.l a2,$3C(a4)
 */
void Op2_SetTimer1RandYield1(uint32_t *rand, const u16 **script, Vm2Block *vm)
{
    vm->timer[1] = rand_ticks(rand, script);
    yield1(script, vm);
}
