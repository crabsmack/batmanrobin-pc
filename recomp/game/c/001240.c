#include "game.h"

/* Op2_ClearNewBlock - Op2_AllocSubBlock's shared tail: clears the 64 bytes (16 longs) of the
 * block, then NEXT. Not an op of its own.
 * In:  block = the object's sub-block (a0)
 * Out: d0 = 0; the block's first 64 bytes = 0; NEXT
 * Evidence: moveq #0,d0 / 16x move.l d0,(a0)+ / NEXT; both paths of Op2_AllocSubBlock bra here;
 *           20 TAS frames
 */
void Op2_ClearNewBlock(uint32_t *d0, u32 *block, const u16 **script)
{
    *d0 = 0;
    for (int i = 0; i < 16; i++)
        block[i] = 0u;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_ResetA3 - a2 op: the motion block (a3) is the object's own first block again ($8(a6)),
 * undoing Op2_LoadA3Handle.
 * In:  self->motion
 * Out: a3 = the block (sign-extended handle); NEXT
 * Evidence: movea.w $8(a6),a3 / NEXT; executed in the TAS
 */
void Op2_ResetA3(const u16 **script, uint32_t *a3, const Vm2Obj *self)
{
    *a3 = (uint32_t)(int32_t)self->motion;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
