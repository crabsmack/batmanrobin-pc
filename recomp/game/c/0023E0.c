#include "game.h"

/* Op2_CallSlot2Native, Op2_StepCoroutine2 with its no-push entry Op2_EnterCoroutine2, and
 * Op2_StopCoroutine3 ($0023E0-$0023FF). */

/* Scr2_002D28: a script that is just the return op ($2D2A): a coroutine slot's idle value */
static constexpr uint32_t Scr2_002D28 = 0x002D28u;

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_CallSlot2Native - a2 op: calls coroutine slot 2 as a native 68000 routine, then continues
 * the script with the registers the routine left.
 * In:  vm->coroutine[2] = the routine; every register the routine reads
 * Out: d0-d7, a1, script as the routine leaves them; handler = the next op's; NEXT
 * Evidence: movea.l $40(a4),a0 / jsr (a0) / NEXT; dispatched in 8 runs, TAS f10512-12924 (the
 *           stage 1 loop at $1E7B4, slot 2 = $1EA24 Boss_Claw_SwingAtCraneStep)
 */
void Op2_CallSlot2Native(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                         uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm,
                         uint32_t a6)
{
    *a0 = vm->coroutine[2].raw();
    Slot2NativeFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, vm, a6);
    next(a0, script);
}

/* script = coroutine slot 2's, then NEXT into its first op */
static void enter_slot2(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *script = vm->coroutine[2];
    next(handler, script);
}

/* Op2_StepCoroutine2 - a2 op: runs coroutine slot 2 for one step: pushes the current script and
 * continues at the slot's (back through the return op $2D2A, which pops it).
 * In:  vm->coroutine[2]
 * Out: (sp) = the stepping script; then as Op2_EnterCoroutine2
 * Evidence: move.l a2,-(a7) / movea.l $40(a4),a2 / NEXT; dispatched in 151 runs
 */
void Op2_StepCoroutine2(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    dk::push_long(dk::addr(*script));
    enter_slot2(handler, script, vm);
}

/* Op2_EnterCoroutine2 - a2 op: continues the script at coroutine slot 2 without pushing the
 * current script (Op2_StepCoroutine2 past its push): the return op that ends the step pops the
 * script an earlier Op2_StepCoroutineN pushed.
 * In:  vm->coroutine[2]
 * Out: script = the slot's; handler = its first op's; NEXT
 * Evidence: movea.l $40(a4),a2 / NEXT; script handler words $23EC dispatched from Op2_SetSpeed's
 *           NEXT ($B0C), 1,012 call-site log entries over the corpus
 */
void Op2_EnterCoroutine2(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    enter_slot2(handler, script, vm);
}

/* Op2_StopCoroutine3 - a2 op: stops coroutine slot 3 by pointing it at the idle script (just
 * the return op).
 * In:  -
 * Out: vm->coroutine[3] = Scr2_002D28; NEXT
 * Evidence: move.l #$2D28,$44(a4) / NEXT; dispatched in 5 runs
 */
void Op2_StopCoroutine3(const u16 **script, Vm2Block *vm)
{
    vm->coroutine[3].v = Scr2_002D28;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
