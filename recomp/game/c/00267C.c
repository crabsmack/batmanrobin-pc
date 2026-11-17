#include "game.h"

/* Child_GetA4 and Child_GetA4Skip2 ($00267C-$002699): the child-object ops' helpers. A word
 * operand names an a6 field holding a child object's handle; with a child they end in
 * Child_A4OfHandle (its a4 block back to the op); without one the op does nothing: the helper
 * returns past it, straight into the script's next op. (The 4-byte-operand twin at $269A is raw
 * bytes, never run.) */

/* the child handle at a6 + the field offset (move.w (a6,d0.w),d1) */
static uint16_t child_field(const Vm2Obj *self, uint16_t field)
{
    return *(const u16 *)((const u8 *)self + (int16_t)field);
}

/* Child_GetA4 - reads the child field operand; with a child, its a4 block for the op; with none,
 * drops the op's return and continues the script (the op does nothing).
 * In:  script = word a6 field offset; self = the parent
 * Out: field = the offset; child = the child's handle; then Child_A4OfHandle (a0 = its a4).
 *      No child: the op's return dropped; handler = the next op's; NEXT
 * Evidence: move.w (a2)+,d0 / move.w (a6,d0.w),d1 / bne.s Child_A4OfHandle / addq.l #4,a7 /
 *           NEXT; bsr'd by Op2_StopChildCoroutine1; 10 runs
 */
void Child_GetA4(uint16_t *field, uint16_t *child, uint32_t *a0, const u16 **script, const Vm2Obj *self)
{
    *field = *(*script)++;
    *child = child_field(self, *field);
    if (*child != 0) {
        dk::jump(at_Child_A4OfHandle);
        return;
    }
    dk::drop_return();
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}

/* Child_GetA4Skip2 - as Child_GetA4 for the child ops with one more word operand: with no child
 * it also skips that word before continuing the script.
 * In:  script = word a6 field offset, then the op's own word operand; self = the parent
 * Out: field, child; then Child_A4OfHandle (a0 = the child's a4, script at the op's operand).
 *      No child: script past the operand, the op's return dropped; NEXT
 * Evidence: move.w (a2)+,d0 / move.w (a6,d0.w),d1 / bne.s Child_A4OfHandle / addq.l #2,a2 /
 *           addq.l #4,a7 / NEXT; bsr'd by Op2_StartChildCoroutine0Rel..3Rel; 32 runs
 */
void Child_GetA4Skip2(uint16_t *field, uint16_t *child, uint32_t *a0, const u16 **script, const Vm2Obj *self)
{
    *field = *(*script)++;
    *child = child_field(self, *field);
    if (*child != 0) {
        dk::jump(at_Child_A4OfHandle);
        return;
    }
    (*script)++;
    dk::drop_return();
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}
