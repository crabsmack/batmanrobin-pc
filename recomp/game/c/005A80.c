#include "game.h"

/* Op4_ScatterVelY, Op4_RandVelY, Op4_RangeVelY, Op4_JitterVelRadius ($005A80-$005AD5): random
 * kicks to the object's y and radius velocities (8.8 px/frame operands). */

/* the random part of the 8.8 velocity ops: Rand_Next's low word shifted right 4, arithmetic
   (signed [-$800, $800): the jitter, [-w/2, w/2)) or logical ([0, $1000): rand / range, [0, w)),
   times w (muls.w), asr.l #4; as 16.16 */
static uint32_t jitter_8_8(const u16 **script)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    return (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
}
static uint32_t rand_8_8(const u16 **script)
{
    uint16_t r = (uint16_t)(uint32_t)Rand_Next();
    return (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
}

/* an 8.8 operand word as 16.16: ext.l / asl.l #8 */
static uint32_t word_8_8(const u16 **script)
{
    return (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
}

/* the next handler word (NEXT) */
static uint32_t next_op(const u16 **script)
{
    return dk::code((int16_t)(uint16_t)*(*script)++);
}

/* Op4_ScatterVelY - a4 op: kicks the object's y velocity by base + a random [0, w/2) (8.8) up or
 * down at random: the signed jitter's sign picks the direction of the base.
 * In:  script = word base (8.8), word w
 * Out: d1 = +-base as 16.16; d0 = the kick; self->vel_y; NEXT
 * Evidence: the Op4_ScatterVelX code on $42(a6); script use $1322C (Drop child B); 29 corpus runs
 */
void Op4_ScatterVelY(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self)
{
    *d1 = word_8_8(script);
    *d0 = jitter_8_8(script);
    if ((int32_t)*d0 < 0)
        *d1 = 0u - *d1;
    *d0 += *d1;
    vm4_add_long(self->vel_y, *d0);
    dk::jump(next_op(script));
}

/* Op4_RandVelY - a4 op: adds a random [0, w) (8.8 px/frame; w may be negative) to the object's y
 * velocity.
 * In:  script = word w
 * Out: self->vel_y; NEXT
 * Evidence: bsr Rand_Next / lsr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4 / add.l d0,$42(a6); script use
 *           $1F1F6 (`5A9E FE00`: up to -2 px/frame); 3 corpus runs
 */
void Op4_RandVelY(const u16 **script, Vm4Obj *self)
{
    vm4_add_long(self->vel_y, rand_8_8(script));
    dk::jump(next_op(script));
}

/* Op4_RangeVelY - a4 op: adds base + a random [0, w) (8.8 px/frame) to the object's y velocity.
 * In:  script = word base (8.8), word w
 * Out: d1 = base as 16.16; d0 = the amount; self->vel_y; NEXT
 * Evidence: the Op4_RangeVelX code on $42(a6); script uses $13256 (Drop child A, `0100 0100`),
 *           $2438A (`FA00 FC00`: a toss upward); 41 corpus runs
 */
void Op4_RangeVelY(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self)
{
    *d1 = word_8_8(script);
    *d0 = rand_8_8(script) + *d1;
    vm4_add_long(self->vel_y, *d0);
    dk::jump(next_op(script));
}

/* Op4_JitterVelRadius - a4 op: adds a random [-w/2, w/2) (8.8) to the object's radius velocity.
 * In:  script = word w
 * Out: d0 = the amount; self->vel_radius; NEXT
 * Evidence: bsr Rand_Next / asr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4 / add.l d0,$36(a6); script uses
 *           $26BB4, $26BD2; 6 corpus runs
 */
void Op4_JitterVelRadius(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = jitter_8_8(script);
    vm4_add_long(self->vel_radius, *d0);
    dk::jump(next_op(script));
}
