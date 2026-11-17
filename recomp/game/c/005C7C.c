#include "game.h"

/* Op4_RangeAccelRadius - a4 op: adds base + a random [0, w) (8.8) to the object's radius
 * acceleration: base << 8 plus (Rand_Next's low word lsr #4) * w, asr.l #4.
 * In:  script = word base (8.8), word w
 * Out: d1 = base as 16.16; d0 = the amount; self->acc_radius; NEXT
 * Evidence: bsr Rand_Next / move.w (a4)+,d1 / ext.l / asl.l #8 / lsr.w #4,d0 / muls.w (a4)+,d0 /
 *           asr.l #4 / add.l d1,d0 / add.l d0,$46(a6); script use $26AB8 (after Op4_RandVelRadius);
 *           7 corpus runs
 */
void Op4_RangeAccelRadius(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self)
{
    uint16_t r = (uint16_t)(uint32_t)Rand_Next();
    *d1 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    int32_t part = ((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4;
    *d0 = (uint32_t)part + *d1;
    vm4_add_long(self->acc_radius, *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
