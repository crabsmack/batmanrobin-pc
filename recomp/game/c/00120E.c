#include "game.h"

/* Op2_AllocSubBlock and Op2_LoadA3Handle ($00120E-$00123F): an object can own extra pool blocks,
 * their handles in a6 fields named by a word operand (a byte offset into the object). */

/* the handle word at a6 + offset (move.w (a6,d1.w)) */
static s16 &handle_field(Vm2Obj *self, uint16_t offset)
{
    return *(s16 *)((u8 *)self + (int16_t)offset);
}

/* Op2_AllocSubBlock - a2 op: makes sure the object owns a pool block in the handle field the
 * operand names (allocating one with ObjPool_Alloc when the field is 0), then clears the block's
 * first 64 bytes (Op2_ClearNewBlock). With the pool empty the script backs up to this op and
 * yields, to retry next frame.
 * In:  script = word field offset; self
 * Out: field = the offset; a0 = the block, then Op2_ClearNewBlock; pool empty: d0 = -1, script
 *      = this op's word, Op2_Yield
 * Evidence: move.w (a2)+,d1 / move.w (a6,d1.w),d0 / bne / bsr ObjPool_Alloc / bne (subq.l #4,a2 /
 *           bra.w Op2_Yield) / move.w a0,(a6,d1.w) / bra.s Op2_ClearNewBlock; 20 TAS frames
 */
void Op2_AllocSubBlock(uint32_t *d0, uint16_t *field, uint32_t *a0, const u16 **script, uint32_t a4, Vm2Obj *self)
{
    *field = *(*script)++;
    s16 &h = handle_field(self, *field);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)h;
    if ((uint16_t)*d0 != 0) {
        *a0 = (uint32_t)(int32_t)(int16_t)(uint16_t)*d0;   /* movea.w: the handle, sign-extended */
        dk::jump(at_Op2_ClearNewBlock);
        return;
    }
    ObjPool_Alloc(d0, a0);
    if (*d0 != 0) {
        *script -= 2;   /* back to the op word: the yield resumes on this op */
        dk::jump(at_Op2_Yield);
        return;
    }
    h = (int16_t)(uint16_t)*a0;
    dk::jump(at_Op2_ClearNewBlock);
}

/* Op2_LoadA3Handle - a2 op: the block whose handle is in the a6 field the operand names becomes
 * the current motion block (a3), so the motion ops act on it.
 * In:  script = word field offset; self
 * Out: field = the offset; a3 = the block (sign-extended handle); NEXT
 * Evidence: move.w (a2)+,d0 / movea.w (a6,d0.w),a3 / NEXT; undone by Op2_ResetA3; executed in the TAS
 */
void Op2_LoadA3Handle(uint16_t *field, const u16 **script, uint32_t *a3, const Vm2Obj *self)
{
    *field = *(*script)++;
    *a3 = (uint32_t)(int32_t)handle_field((Vm2Obj *)self, *field);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
