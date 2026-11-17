#include "game.h"

/* Op2_StartChildCoroutine1Rel ($002B16-$002B25). */

/* Op2_StartChildCoroutine1Rel - a2 op: starts the child object's coroutine slot 1 at a
 * relative address in the parent's script (the offset word's address plus its value); does
 * nothing without a child (Child_GetA4Skip2 then skips the operand and goes on to the next op).
 * In:  script = word a6 field offset (the child), word offset; self = the parent
 * Out: d0/d1 low words = the field offset / the child's handle; a1 = the slot's script; the
 *      child's vm->coroutine[1] = a1; NEXT
 * Evidence: bsr.w Child_GetA4Skip2 / movea.l a2,a1 / adda.w (a2)+,a1 / move.l a1,$3C(a0) /
 *           NEXT; 10 runs
 */
void Op2_StartChildCoroutine1Rel(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                 uint32_t d6, uint32_t d7, uint32_t *a1, const u16 **script, uint32_t a3, uint32_t a4,
                                 uint32_t a5, const Vm2Obj *self)
{
    uint16_t field = (uint16_t)*d0, child = (uint16_t)*d1;
    uint32_t a0 = 0;
    Child_GetA4Skip2(&field, &child, &a0, script, self);   /* no child: never comes back */
    *d0 = (*d0 & 0xFFFF0000u) | field;
    *d1 = (*d1 & 0xFFFF0000u) | child;
    Vm2Block *child_vm = dk::handle<Vm2Block>((int16_t)a0);
    *a1 = dk::addr(*script) + (uint32_t)(int32_t)(int16_t)(uint16_t)**script;
    (*script)++;
    child_vm->coroutine[1].v = *a1;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
