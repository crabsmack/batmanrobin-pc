#include "game.h"

/* Op4_ChildRangeAccelX / Op4_ChildJitterAccelY ($005F4E-$005F79): random kicks to the last
 * spawned child's accelerations, base + [0, w) for x and +-w/2 for y, in 8.8. */

/* add.l d0,<long>: the sum, and the add's condition codes for the caller (X = C = the carry) */
static uint32_t add_long_ccr(uint32_t old, uint32_t d, dk::Flags *ccr)
{
    uint32_t sum = old + d;
    bool carry = sum < old;
    *ccr = dk::Flags{ carry, (sum >> 31) != 0, sum == 0, ((~(old ^ d)) & (old ^ sum) & 0x80000000u) != 0, carry };
    return sum;
}

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op4_ChildRangeAccelX - a4 op: adds base + a random [0, w) (8.8) to the child's x acceleration.
 * In:  script = word base (8.8), word w (8.8); child = a5
 * Out: d1 = base (16.16); d0 = the amount; child->acc_x; the add.l's condition codes; NEXT
 * Evidence: bsr Rand_Next / move.w (a4)+,d1 / ext.l / asl.l #8 / lsr.w #4,d0 / muls.w (a4)+,d0 /
 *           asr.l #4 / add.l d1,d0 / add.l d0,$4E(a5); script use $28F56 (`5F4E FFC8 0013`); 1
 *           corpus run
 */
void Op4_ChildRangeAccelX(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child,
                          dk::Flags *ccr)
{
    uint16_t r = (uint16_t)(uint32_t)Rand_Next();
    *d1 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    *d0 = (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4) + *d1;
    child->acc_x = (int32_t)add_long_ccr((uint32_t)(int32_t)child->acc_x, *d0, ccr);
    next(handler, script);
}

/* Op4_ChildJitterAccelY - a4 op: adds a random [-w/2, w/2) (8.8) to the child's y acceleration.
 * In:  script = word w (8.8); child = a5
 * Out: d0 = the amount (16.16); child->acc_y; the add.l's condition codes; NEXT
 * Evidence: bsr Rand_Next / asr.w #4,d0 / muls.w (a4)+,d0 / asr.l #4,d0 / add.l d0,$52(a5); script
 *           uses $28F5C (`5F68 000C`), $28F8A; 1 corpus run
 */
void Op4_ChildJitterAccelY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    *d0 = (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
    child->acc_y = (int32_t)add_long_ccr((uint32_t)(int32_t)child->acc_y, *d0, ccr);
    next(handler, script);
}
