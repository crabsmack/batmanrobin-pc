#include "game.h"

/* The start ops of coroutine slot 0 (a4 +$38): here, relative, absolute ($002334-$00234F). A
 * slot is then run a step at a time by Op2_StepCoroutine0. */

/* Op2_StartCoroutine0Here - a2 op: points coroutine slot 0 at the next op and continues there,
 * so the ops that follow run now and again on every Op2_StepCoroutine0 (the restart point of a
 * coroutine body that ends in the return op).
 * In:  -
 * Out: vm->coroutine[0] = script; handler = the next op's; NEXT
 * Evidence: move.l a2,$38(a4) / NEXT; dispatched in all 153 corpus runs; e.g. $1B040
 *           `2334 ... 2D2A`
 */
void Op2_StartCoroutine0Here(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    vm->coroutine[0] = *script;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_StartCoroutine0Rel - a2 op: starts coroutine slot 0 at a relative script address (the
 * operand's address plus its value).
 * In:  script = word offset (from the offset word)
 * Out: vm->coroutine[0]; handler = the next op's; NEXT after the operand
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,$38(a4) / NEXT; dispatched in 153 runs;
 *           $1E79C `233C 0032` (stage 1, TAS f10512-12924)
 */
void Op2_StartCoroutine0Rel(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    vm->coroutine[0] = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_StartCoroutine0Abs - a2 op: starts coroutine slot 0 at an absolute script address.
 * In:  script = long address
 * Out: vm->coroutine[0]; NEXT after the operand
 * Evidence: move.l (a2)+,$38(a4) / NEXT; dispatched in 31 runs incl. the TAS (e.g. $1B9C8
 *           `2348 0001B9DC`)
 */
void Op2_StartCoroutine0Abs(const u16 **script, Vm2Block *vm)
{
    vm->coroutine[0].v = (uint32_t)(*script)[0] << 16 | (*script)[1];
    *script += 2;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
