#include "game.h"

/* The start ops of coroutine slot 1 (a4 +$3C): here and relative ($00237C-$00238F; the
 * absolute twin at $2390 is raw bytes, never run). */

/* Op2_StartCoroutine1Here - a2 op: points coroutine slot 1 at the next op and continues there
 * (the restart point of the coroutine body that follows; it can be native code after
 * Op2_Native).
 * In:  -
 * Out: vm->coroutine[1] = script; handler = the next op's; NEXT
 * Evidence: move.l a2,$3C(a4) / NEXT; dispatched in 83 runs; $1E860 `237C 07FC`
 */
void Op2_StartCoroutine1Here(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    vm->coroutine[1] = *script;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_StartCoroutine1Rel - a2 op: starts coroutine slot 1 at a relative script address (the
 * operand's address plus its value).
 * In:  script = word offset (from the offset word)
 * Out: vm->coroutine[1]; handler = the next op's; NEXT after the operand
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,$3C(a4) / NEXT; dispatched in 112 runs
 */
void Op2_StartCoroutine1Rel(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    vm->coroutine[1] = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
