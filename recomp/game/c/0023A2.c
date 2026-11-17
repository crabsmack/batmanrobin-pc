#include "game.h"

/* Op2_StepCoroutine1, its no-push entry Op2_EnterCoroutine1, and Op2_StopCoroutine2
 * ($0023A2-$0023B5). */

/* Scr2_002D28: a script that is just the return op ($2D2A): a coroutine slot's idle value */
static constexpr uint32_t Scr2_002D28 = 0x002D28u;

/* script = coroutine slot 1's, then NEXT into its first op */
static void enter_slot1(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *script = vm->coroutine[1];
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_StepCoroutine1 - a2 op: runs coroutine slot 1 for one step: pushes the current script
 * and continues at the slot's (back through the return op $2D2A, which pops it).
 * In:  vm->coroutine[1]
 * Out: (sp) = the stepping script; then as Op2_EnterCoroutine1
 * Evidence: move.l a2,-(a7) / movea.l $3C(a4),a2 / NEXT; dispatched in 131 runs, 98k TAS frames
 */
void Op2_StepCoroutine1(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    dk::push_long(dk::addr(*script));
    enter_slot1(handler, script, vm);
}

/* Op2_EnterCoroutine1 - a2 op: continues the script at coroutine slot 1 without pushing the
 * current script (Op2_StepCoroutine1 past its push): the return op that ends the step pops the
 * script an earlier Op2_StepCoroutineN pushed.
 * In:  vm->coroutine[1]
 * Out: script = the slot's; handler = its first op's; NEXT
 * Evidence: movea.l $3C(a4),a2 / NEXT; script handler words $23A4 dispatched from Op2_SkipWord's
 *           NEXT ($7E8), 487,711 call-site log entries over the corpus
 */
void Op2_EnterCoroutine1(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    enter_slot1(handler, script, vm);
}

/* Op2_StopCoroutine2 - a2 op: stops coroutine slot 2 by pointing it at the idle script (just
 * the return op).
 * In:  -
 * Out: vm->coroutine[2] = Scr2_002D28; NEXT
 * Evidence: move.l #$2D28,$40(a4) / NEXT; dispatched in 132 runs
 */
void Op2_StopCoroutine2(const u16 **script, Vm2Block *vm)
{
    vm->coroutine[2].v = Scr2_002D28;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
