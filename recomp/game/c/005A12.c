#include "game.h"

/* Op4_JitterVelX / Op4_ScatterVelX ($005A12-$005A3F): random kicks to the object's x velocity. */

/* the jitter of the 8.8 velocity ops: Rand_Next's low word asr #4 (signed, [-$800, $800)) times
   w (muls.w), asr.l #4: [-w/2, w/2) in 8.8, as 16.16 */
static uint32_t jitter_8_8(const u16 **script)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    return (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
}

/* an 8.8 operand word as 16.16: ext.l / asl.l #8 */
static uint32_t word_8_8(const u16 **script)
{
    return (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
}

/* Op4_JitterVelX - a4 op: adds a random [-w/2, w/2) (8.8 px/frame) to the object's x velocity.
 * In:  script = word w
 * Out: d0 = the amount (16.16); self->vel_x; NEXT
 * Evidence: bsr Rand_Next / asr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4,d0 / add.l d0,$3E(a6); 6 runs
 */
void Op4_JitterVelX(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = jitter_8_8(script);
    vm4_add_long(self->vel_x, *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op4_ScatterVelX - a4 op: kicks the object's x velocity by base + a random [0, w/2) (8.8) in a
 * random direction: the signed jitter's sign picks the direction of the base.
 * In:  script = word base (8.8), word w
 * Out: d1 = +-base as 16.16; d0 = the kick; self->vel_x; NEXT
 * Evidence: bsr Rand_Next / move.w (a4)+,d1 / ext.l / asl.l #8 / asr.w #4,d0 / muls.w (a4)+,d0 /
 *           asr.l #4 / bpl / neg.l d1 / add.l d1,d0 / add.l d0,$3E(a6); script uses $13262 (`5A24 0033
 *           0080` in Drop child A), $1F1FA, $24396; 33 corpus runs
 */
void Op4_ScatterVelX(uint32_t *d0, uint32_t *d1, const u16 **script, Vm4Obj *self)
{
    *d1 = word_8_8(script);
    *d0 = jitter_8_8(script);
    if ((int32_t)*d0 < 0)
        *d1 = 0u - *d1;
    *d0 += *d1;
    vm4_add_long(self->vel_x, *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
