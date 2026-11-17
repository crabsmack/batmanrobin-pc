#include "game.h"

/* Op4_RandVelRadius / Op4_RangeVelRadius ($005AFA-$005B21): random additions to the object's
 * radius velocity (8.8 operands). */

/* Rand_Next's low word lsr #4 ([0, $1000)) times w (muls.w), asr.l #4: [0, w) in 8.8, as 16.16 */
static uint32_t rand_8_8(const u16 **script)
{
    uint16_t r = (uint16_t)(uint32_t)Rand_Next();
    return (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
}

/* Op4_RandVelRadius - a4 op: adds a random [0, w) (8.8) to the object's radius velocity.
 * In:  script = word w
 * Out: self->vel_radius; NEXT
 * Evidence: bsr Rand_Next / lsr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4 / add.l d0,$36(a6); script use
 *           $26AB4; 7 corpus runs
 */
void Op4_RandVelRadius(const u16 **script, Vm4Obj *self)
{
    vm4_add_long(self->vel_radius, rand_8_8(script));
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op4_RangeVelRadius - a4 op: adds base + a random [0, w) (8.8) to the object's radius velocity.
 * In:  script = word base (8.8), word w
 * Out: d1 = base as 16.16; d0 = the amount; self->vel_radius; NEXT
 * Evidence: base << 8 + (Rand_Next lsr.w #4) * w >> 4 added to $36(a6); script use $36026 (after
 *           Op4_JitterHeading); 17 corpus runs
 */
void Op4_RangeVelRadius(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self)
{
    *d1 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    *d0 = rand_8_8(script) + *d1;
    vm4_add_long(self->vel_radius, *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
