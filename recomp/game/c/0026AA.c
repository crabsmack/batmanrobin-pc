#include "game.h"

/* Child_A4OfHandle ($0026AA-$0026B1). */

/* Child_A4OfHandle - the shared tail of the Child_GetA4 helpers: the child object's a4 (VM)
 * block, back to the op.
 * In:  child = the child object's handle
 * Out: its a4 block (the handle at child +$A, sign-extended)
 * Evidence: movea.w d1,a0 / movea.w $A(a0),a0 / rts; a script object's +$A holds its a4 block
 *           (Obj_InitSpawned); 32 runs
 */
uint32_t Child_A4OfHandle(uint16_t child)
{
    const Vm2Obj *obj = dk::handle<Vm2Obj>((int16_t)child);
    return (uint32_t)(int32_t)obj->vm;
}
