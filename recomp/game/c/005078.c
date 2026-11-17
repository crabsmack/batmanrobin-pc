#include "game.h"

/* Op2_ClearMotionBlock - a2 op: zeroes the 64 bytes of the object's motion block (the block a3 is
 * reset to by Op2_ResetA3), then NEXT.
 * In:  self = a6 (its motion handle +$8); script = the next op
 * Out: d0 = 0; the block cleared; a0 = the next op's handler (NEXT); the moveq's condition codes
 * Evidence: movea.w $8(a6),a0 / moveq #0,d0 / 16 x move.l d0,(a0)+ / NEXT; never run in the corpus
 *           (in the a2 handler range between Op2_AllocOsc and Op2_StopOsc); purpose from the code
 */
void Op2_ClearMotionBlock(uint32_t *d0, uint32_t *a0, const u16 **script, const Vm2Obj *self, dk::Flags *ccr)
{
    u32 *block = dk::handle<u32>(self->motion);
    *d0 = 0;
    for (int i = 0; i < 16; i++)
        block[i] = 0u;
    *ccr = dk::Flags::test(0, 4, ccr->x);
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}
