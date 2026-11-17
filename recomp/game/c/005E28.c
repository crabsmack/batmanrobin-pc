#include "game.h"

/* Op4_ChildRandVelY / Op4_ChildRangeVelY ($005E28-$005E53): random kicks to the last spawned
 * child's y velocity, [0, w) and base + [0, w) in 8.8 px/frame. */

/* add.l d0,<long>: the sum, and the add's condition codes for the caller (X = C = the carry) */
static uint32_t add_long_ccr(uint32_t old, uint32_t d, dk::Flags *ccr)
{
    uint32_t sum = old + d;
    bool carry = sum < old;
    *ccr = dk::Flags{ carry, (sum >> 31) != 0, sum == 0, ((~(old ^ d)) & (old ^ sum) & 0x80000000u) != 0, carry };
    return sum;
}

/* Rand_Next's low word lsr #4 ([0, $1000)) times w (muls.w), asr.l #4: [0, w) in 8.8, as 16.16 */
static uint32_t rand_8_8(uint16_t r, const u16 **script)
{
    return (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
}

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op4_ChildRandVelY - a4 op: adds a random [0, w) (8.8 px/frame; w may be negative) to the child's
 * y velocity.
 * In:  script = word w (8.8); child = a5
 * Out: d0 = the amount (16.16); child->vel_y; the add.l's condition codes; NEXT
 * Evidence: bsr Rand_Next / lsr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4,d0 / add.l d0,$42(a5); 26
 *           a4-script uses, e.g. $1A63C (`5E28 FA00`), $1A68A (`FB00`); 39 corpus runs
 */
void Op4_ChildRandVelY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    *d0 = rand_8_8((uint16_t)(uint32_t)Rand_Next(), script);
    child->vel_y = (int32_t)add_long_ccr((uint32_t)(int32_t)child->vel_y, *d0, ccr);
    next(handler, script);
}

/* Op4_ChildRangeVelY - a4 op: adds base + a random [0, w) (8.8 px/frame) to the child's y velocity.
 * In:  script = word base (8.8), word w (8.8); child = a5
 * Out: d1 = base (16.16); d0 = the amount; child->vel_y; the add.l's condition codes; NEXT
 * Evidence: the Op4_RangeVelY code on $42(a5); script uses $10D9E / $10DB0 (`FFCD FFCD`, skid dust:
 *           a small upward hop), $1A3AA; 95 corpus runs
 */
void Op4_ChildRangeVelY(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child,
                        dk::Flags *ccr)
{
    uint16_t r = (uint16_t)(uint32_t)Rand_Next();
    *d1 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    *d0 = rand_8_8(r, script) + *d1;
    child->vel_y = (int32_t)add_long_ccr((uint32_t)(int32_t)child->vel_y, *d0, ccr);
    next(handler, script);
}
