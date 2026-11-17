#include "game.h"

/* the stepping script's a2, which Op2_StepCoroutineN pushed before entering the coroutine */
static const u16 *pop_script(void)
{
    PTR(const u16) caller;
    caller.v = dk::pop_long();
    return caller;
}

/* Timer 0's ops and the coroutine waits ($000E3E-$000F2D): coroutine 3's random set-and-yield,
 * the four Op2_WaitTimerN, and timer 0's set / tick-and-branch / tick-and-goto-vector ops. A timer
 * counts down by the VM block's tick (speed >> 4) each time its op runs. */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
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

/* move.w $30(a4),d0 / sub.w d0,timer / bhi: the timer loses one tick; true while it is still
   running (it was above the tick: no borrow, not zero) */
static bool tick_down(u16 &timer, uint16_t *tick, const Vm2Block *vm)
{
    *tick = vm->tick;
    uint16_t t = timer;
    timer = (uint16_t)(t - *tick);
    return t > *tick;
}

/* a coroutine's wait: while the timer runs, back to the stepping script (Op2_Return) with the
   slot unmoved, so the op runs again on the next step; at 0 it is cleared and the coroutine goes on */
static void wait_timer(int n, uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    if (tick_down(vm->timer[n], tick, vm)) {
        dk::jump(at_Op2_Return);
        return;
    }
    vm->timer[n] = 0;
    next(handler, script);
}

/* Op2_SetTimer3RandYield3 - a2 op for coroutine 3: timer 3 = base + random below range, then the
 * coroutine yields (slot 3 resumes at the next op) and the stepping script continues.
 * In:  script = words range, base; the stepping script's a2 on the stack
 * Out: vm->timer[3], vm->coroutine[3]; rand's low word = the ticks; script = the popped one, NEXT
 * Evidence: move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 / move.w d0,$20(a4) /
 *           move.l a2,$44(a4) / movea.l (a7)+,a2 / NEXT
 */
void Op2_SetTimer3RandYield3(uint32_t *rand, const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    vm->timer[3] = rand_ticks(rand, script);
    vm->coroutine[3] = *script;
    *script = pop_script();
    next(&handler, script);
}

/* Op2_WaitTimer0 - a2 op for a coroutine: waits for timer 0 (one tick per step).
 * In:  vm->timer[0], vm->tick
 * Out: still running: Op2_Return (the slot unmoved); else timer cleared, NEXT; tick = vm->tick
 * Evidence: move.w $30(a4),d0 / sub.w d0,$1A(a4) / bhi.w Op2_Return / clr.w $1A(a4) / NEXT
 */
void Op2_WaitTimer0(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    wait_timer(0, tick, handler, script, vm);
}

/* Op2_WaitTimer1 - a2 op for a coroutine: waits for timer 1 (one tick per step).
 * In:  vm->timer[1], vm->tick
 * Out: still running: Op2_Return; else timer cleared, NEXT; tick = vm->tick
 * Evidence: sub.w d0,$1C(a4) / bhi.w Op2_Return / clr.w $1C(a4) / NEXT
 */
void Op2_WaitTimer1(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    wait_timer(1, tick, handler, script, vm);
}

/* Op2_WaitTimer2 - a2 op for a coroutine: waits for timer 2 (one tick per step).
 * In:  vm->timer[2], vm->tick
 * Out: still running: Op2_Return; else timer cleared, NEXT; tick = vm->tick
 * Evidence: sub.w d0,$1E(a4) / bhi.w Op2_Return / clr.w $1E(a4) / NEXT
 */
void Op2_WaitTimer2(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    wait_timer(2, tick, handler, script, vm);
}

/* Op2_WaitTimer3 - a2 op for a coroutine: waits for timer 3 (one tick per step).
 * In:  vm->timer[3], vm->tick
 * Out: still running: Op2_Return; else timer cleared, NEXT; tick = vm->tick
 * Evidence: sub.w d0,$20(a4) / bhi.w Op2_Return / clr.w $20(a4) / NEXT
 */
void Op2_WaitTimer3(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    wait_timer(3, tick, handler, script, vm);
}

/* Op2_SetTimer0 - a2 op: timer 0 = the operand.
 * In:  script = word ticks
 * Out: vm->timer[0]; NEXT
 * Evidence: move.w (a2)+,$1A(a4) / NEXT
 */
void Op2_SetTimer0(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    vm->timer[0] = *(*script)++;
    next(handler, script);
}

/* Op2_SetTimer0Rand - a2 op: timer 0 = base + a random value below range (Rand_Below).
 * In:  script = words range, base
 * Out: vm->timer[0]; rand's low word = the ticks (its upper word Rand_Below's); NEXT
 * Evidence: move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 / move.w d0,$1A(a4) / NEXT
 */
void Op2_SetTimer0Rand(uint32_t *rand, const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    vm->timer[0] = rand_ticks(rand, script);
    next(&handler, script);
}

/* Op2_TickTimer0Branch - a2 op: timer 0 loses a tick; branches when it runs out this run. A timer
 * already at 0 does not tick and does not branch.
 * In:  script = a word offset; vm->timer[0], vm->tick
 * Out: at 0 already, or still running: Op2_SkipWord (NEXT after the offset); expired: timer
 *      cleared, the branch by the offset
 * Evidence: tst.w $1A(a4) / beq.w Op2_SkipWord, then Op2_TickTimer0BranchNoZeroTest's code
 */
void Op2_TickTimer0Branch(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    if (vm->timer[0] == 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    Op2_TickTimer0BranchNoZeroTest(tick, handler, script, vm);
}

/* Op2_TickTimer0BranchNoZeroTest - a2 op: Op2_TickTimer0Branch without its zero test: a timer at
 * 0 borrows and so branches too.
 * In:  script = a word offset; vm->timer[0], vm->tick
 * Out: still running: Op2_SkipWord; else timer cleared, the branch by the offset (adda.w (a2),a2)
 * Evidence: move.w $30(a4),d0 / sub.w d0,$1A(a4) / bhi.w Op2_SkipWord / clr.w / adda.w (a2),a2 /
 *           NEXT; a computed-jump target in the corpus
 */
void Op2_TickTimer0BranchNoZeroTest(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    if (tick_down(vm->timer[0], tick, vm)) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    vm->timer[0] = 0;
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}

/* Op2_TickTimer0GotoV0 - a2 op: timer 0 loses a tick; when it runs out the script continues at
 * the object's script vector 0.
 * In:  vm->timer[0], vm->tick; self->vector[0]
 * Out: still running: VM2_Next; else timer cleared, script = vector 0, NEXT
 * Evidence: sub.w d0,$1A(a4) / bhi.w VM2_Next / clr.w / movea.l $32(a6),a2 / NEXT
 */
void Op2_TickTimer0GotoV0(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm, const Vm2Obj *self)
{
    if (tick_down(vm->timer[0], tick, vm)) {
        dk::jump(at_VM2_Next);
        return;
    }
    vm->timer[0] = 0;
    *script = self->vector[0];
    next(handler, script);
}

/* Op2_TickTimer0GotoV1 - a2 op: timer 0 loses a tick; when it runs out the script continues at
 * the object's script vector 1.
 * In:  vm->timer[0], vm->tick; self->vector[1]
 * Out: still running: VM2_Next; else timer cleared, script = vector 1, NEXT
 * Evidence: sub.w d0,$1A(a4) / bhi.w VM2_Next / clr.w / movea.l $36(a6),a2 / NEXT
 */
void Op2_TickTimer0GotoV1(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm, const Vm2Obj *self)
{
    if (tick_down(vm->timer[0], tick, vm)) {
        dk::jump(at_VM2_Next);
        return;
    }
    vm->timer[0] = 0;
    *script = self->vector[1];
    next(handler, script);
}
