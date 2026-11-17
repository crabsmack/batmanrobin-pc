#include "game.h"

/* Op4_RangeAccelX / Op4_JitterAccelY ($005BC4-$005BED): random additions to the object's
 * accelerations (8.8 operands). */

/* Op4_RangeAccelX - a4 op: adds base + a random [0, w) (8.8) to the object's x acceleration:
 * base << 8 plus (Rand_Next's low word lsr #4) * w, asr.l #4.
 * In:  script = word base (8.8), word w
 * Out: self->acc_x; NEXT
 * Evidence: bsr Rand_Next / move.w (a4)+,d1 / ext.l / asl.l #8 / lsr.w #4,d0 / muls.w (a4)+,d0 /
 *           asr.l #4 / add.l d1,d0 / add.l d0,$4E(a6); script use $13226 (Drop child B, `FFF8 FFF9`:
 *           a drag); 33 corpus runs
 */
void Op4_RangeAccelX(const u16 **script, Vm4Obj *self)
{
    uint16_t r = (uint16_t)(uint32_t)Rand_Next();
    uint32_t base = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    int32_t part = ((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4;
    vm4_add_long(self->acc_x, (uint32_t)part + base);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op4_JitterAccelY - a4 op: adds a random [-w/2, w/2) (8.8) to the object's y acceleration.
 * In:  script = word w
 * Out: d0 = the amount (16.16); self->acc_y; NEXT (handler = the next op)
 * Evidence: bsr Rand_Next / asr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4 / add.l d0,$52(a6); 13 runs
 */
void Op4_JitterAccelY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *self)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    *d0 = (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
    vm4_add_long(self->acc_y, *d0);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
