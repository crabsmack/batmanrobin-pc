#include "game.h"

/* Op4_ChildScatterVelRadius - a4 op: kicks the last spawned child's radius velocity by base + a
 * random [0, w/2) (8.8), outward or inward at random: the signed jitter's sign picks the
 * direction of the base.
 * In:  script = word base (8.8), word w (8.8); child = a5
 * Out: d1 = +-base (16.16); d0 = the kick; child->vel_radius; the add.l's condition codes; NEXT
 * Evidence: bsr Rand_Next / move.w (a4)+,d1 / ext.l / asl.l #8 / asr.w #4,d0 / muls.w (a4)+,d0 /
 *           asr.l #4 / bpl / neg.l d1 / add.l d1,d0 / add.l d0,$36(a5); script use $26AD8
 *           (`5E66 0100 0100`); 3 corpus runs
 */
void Op4_ChildScatterVelRadius(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child,
                               dk::Flags *ccr)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    *d1 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    *d0 = (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
    if ((int32_t)*d0 < 0)
        *d1 = 0u - *d1;
    *d0 += *d1;
    uint32_t old = (uint32_t)(int32_t)child->vel_radius;
    uint32_t sum = old + *d0;
    bool carry = sum < old;   /* add.l: X = C = the carry */
    *ccr = dk::Flags{ carry, (sum >> 31) != 0, sum == 0, ((~(old ^ *d0)) & (old ^ sum) & 0x80000000u) != 0, carry };
    child->vel_radius = (int32_t)sum;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
