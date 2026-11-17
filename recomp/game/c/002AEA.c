#include "game.h"

/* Op2_StopChildCoroutine1 ($002AEA-$002AF9). */

/* Scr2_002D28: a script that is just the return op ($2D2A): a coroutine slot's idle value */
static constexpr uint32_t Scr2_002D28 = 0x002D28u;

/* Op2_StopChildCoroutine1 - a2 op: stops the child object's coroutine slot 1 (points it at the
 * idle script); does nothing without a child (Child_GetA4 then goes on to the next op).
 * In:  script = word a6 field offset (the child); self = the parent
 * Out: d0/d1 low words = the field offset / the child's handle; the child's vm->coroutine[1] =
 *      Scr2_002D28; NEXT
 * Evidence: bsr.w Child_GetA4 / move.l #$2D28,$3C(a0) / NEXT; 10 runs; $31CF0 `2AEA 002A` before
 *           `2B16 002A`
 */
void Op2_StopChildCoroutine1(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                             uint32_t d6, uint32_t d7, uint32_t a1, const u16 **script, uint32_t a3, uint32_t a4,
                             uint32_t a5, const Vm2Obj *self)
{
    uint16_t field = (uint16_t)*d0, child = (uint16_t)*d1;
    uint32_t a0 = 0;
    Child_GetA4(&field, &child, &a0, script, self);   /* no child: never comes back */
    *d0 = (*d0 & 0xFFFF0000u) | field;
    *d1 = (*d1 & 0xFFFF0000u) | child;
    dk::handle<Vm2Block>((int16_t)a0)->coroutine[1].v = Scr2_002D28;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
