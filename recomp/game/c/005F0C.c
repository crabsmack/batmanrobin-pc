#include "game.h"

/* Op4_ChildJitterAccelX / Op4_ChildScatterAccelX ($005F0C-$005F3B): random kicks to the last
 * spawned child's x acceleration, +-w/2 and +-(base + [0, w/2)) in 8.8. */

/* add.l d0,<long>: the sum, and the add's condition codes for the caller (X = C = the carry) */
static uint32_t add_long_ccr(uint32_t old, uint32_t d, dk::Flags *ccr)
{
    uint32_t sum = old + d;
    bool carry = sum < old;
    *ccr = dk::Flags{ carry, (sum >> 31) != 0, sum == 0, ((~(old ^ d)) & (old ^ sum) & 0x80000000u) != 0, carry };
    return sum;
}

/* the 8.8 jitter: Rand_Next's low word asr #4 times w (muls.w), asr.l #4: [-w/2, w/2), 16.16 */
static uint32_t jitter_8_8(int16_t r, const u16 **script)
{
    return (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
}

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op4_ChildJitterAccelX - a4 op: adds a random [-w/2, w/2) (8.8) to the child's x acceleration.
 * In:  script = word w (8.8); child = a5
 * Out: d0 = the amount (16.16); child->acc_x; the add.l's condition codes; NEXT
 * Evidence: bsr Rand_Next / asr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4,d0 / add.l d0,$4E(a5); script
 *           uses $3582A (`5F0C 0013`), $3CE58; 8 corpus runs
 */
void Op4_ChildJitterAccelX(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    *d0 = jitter_8_8((int16_t)(uint16_t)(uint32_t)Rand_Next(), script);
    child->acc_x = (int32_t)add_long_ccr((uint32_t)(int32_t)child->acc_x, *d0, ccr);
    next(handler, script);
}

/* Op4_ChildScatterAccelX - a4 op: kicks the child's x acceleration by base + a random [0, w/2)
 * (8.8), left or right at random.
 * In:  script = word base (8.8), word w (8.8); child = a5
 * Out: d1 = +-base (16.16); d0 = the kick; child->acc_x; the add.l's condition codes; NEXT
 * Evidence: the Op4_Scatter* code on $4E(a5); script uses $36F98 (`5F1E 0005 0019`), $37D62; 30
 *           corpus runs
 */
void Op4_ChildScatterAccelX(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child,
                            dk::Flags *ccr)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    *d1 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    *d0 = jitter_8_8(r, script);
    if ((int32_t)*d0 < 0)
        *d1 = 0u - *d1;
    *d0 += *d1;
    child->acc_x = (int32_t)add_long_ccr((uint32_t)(int32_t)child->acc_x, *d0, ccr);
    next(handler, script);
}
