#include "game.h"

/* Op2_StepCoroutine0 and Op2_StopCoroutine1 ($00235A-$00237B). */

/* Scr2_002D28: a script that is just the return op ($2D2A): a coroutine slot's idle value */
static constexpr uint32_t Scr2_002D28 = 0x002D28u;

/* Op2_StepCoroutine0 - a2 op: runs coroutine slot 0 for one step: pushes the current script
 * and continues at the slot's; the coroutine comes back through the return op $2D2A (or a
 * SetTimer..Yield0 op that saves its position first), which pops the pushed script.
 * In:  vm->coroutine[0]
 * Out: (sp) = the stepping script; script = the slot's; handler = its first op's; NEXT
 * Evidence: move.l a2,-(a7) / movea.l $38(a4),a2 / NEXT; dispatched in 153 runs, 128k TAS frames
 *           (once per object per frame)
 */
void Op2_StepCoroutine0(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    dk::push_long(dk::addr(*script));
    *script = vm->coroutine[0];
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_StopCoroutine1 - a2 op: stops coroutine slot 1 by pointing it at the idle script (just
 * the return op).
 * In:  -
 * Out: vm->coroutine[1] = Scr2_002D28; handler = the next op's; NEXT
 * Evidence: move.l #$2D28,$3C(a4) / NEXT; dispatched in 78 runs
 */
void Op2_StopCoroutine1(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    vm->coroutine[1].v = Scr2_002D28;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
