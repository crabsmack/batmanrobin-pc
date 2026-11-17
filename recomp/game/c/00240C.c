#include "game.h"

/* The start ops of coroutine slot 3 (a4 +$44): here and relative ($00240C-$00241F; the
 * absolute twin at $2420 is raw bytes, never run). */

/* Op2_StartCoroutine3Here - a2 op: points coroutine slot 3 at the next op and continues there
 * (the restart point of the coroutine body that follows).
 * In:  -
 * Out: vm->coroutine[3] = script; NEXT
 * Evidence: move.l a2,$44(a4) / NEXT; dispatched in 5 runs (TAS from f20399)
 */
void Op2_StartCoroutine3Here(const u16 **script, Vm2Block *vm)
{
    vm->coroutine[3] = *script;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_StartCoroutine3Rel - a2 op: starts coroutine slot 3 at a relative script address (the
 * operand's address plus its value).
 * In:  script = word offset
 * Out: vm->coroutine[3]; NEXT after the operand
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,$44(a4) / NEXT; dispatched in 50 runs
 */
void Op2_StartCoroutine3Rel(const u16 **script, Vm2Block *vm)
{
    vm->coroutine[3] = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    (*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
