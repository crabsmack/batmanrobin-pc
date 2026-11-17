#include "game.h"

/* The start ops of coroutine slot 2 (a4 +$40): here and relative ($0023C4-$0023D7; the
 * absolute twin at $23D8 is raw bytes, never run). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_StartCoroutine2Here - a2 op: points coroutine slot 2 at the next op and continues there
 * (the restart point of the coroutine body that follows).
 * In:  -
 * Out: vm->coroutine[2] = script; handler = the next op's; NEXT
 * Evidence: move.l a2,$40(a4) / NEXT; dispatched in 96 runs
 */
void Op2_StartCoroutine2Here(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    vm->coroutine[2] = *script;
    next(handler, script);
}

/* Op2_StartCoroutine2Rel - a2 op: sets coroutine slot 2 to a relative address (the operand's
 * address plus its value): a coroutine script for Op2_StepCoroutine2, or native code for
 * Op2_CallSlot2Native.
 * In:  script = word offset
 * Out: vm->coroutine[2]; handler = the next op's; NEXT after the operand
 * Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,$40(a4) / NEXT; dispatched in 148 runs;
 *           TAS f11040 an object's slot 2 = $1EA24 (68000 code) under an Op2_CallSlot2Native loop
 */
void Op2_StartCoroutine2Rel(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    vm->coroutine[2] = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    (*script)++;
    next(handler, script);
}
