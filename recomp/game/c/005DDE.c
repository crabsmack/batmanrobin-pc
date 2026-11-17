#include "game.h"

/* Op4_ChildRangeVelX - a4 op: adds base + a random [0, w) (8.8 px/frame) to the last spawned
 * child's x velocity: Rand_Next's low word lsr #4 ([0, $1000)) times w, asr.l #4, plus base << 8.
 * In:  script = word base (8.8), word w (8.8); child = a5
 * Out: d1 = base (16.16); d0 = the amount; child->vel_x; the add.l's condition codes; NEXT
 * Evidence: bsr Rand_Next / move.w (a4)+,d1 / ext.l / asl.l #8 / lsr.w #4,d0 / muls.w (a4)+,d0 /
 *           asr.l #4 / add.l d1,d0 / add.l d0,$3E(a5); script uses $10DA4 / $10DB6 in the skid dust
 *           scripts Scr4_010D9A / Scr4_010DAC (`FF80 FF80` / `0080 0080`), $2530A; 89 corpus runs
 */
void Op4_ChildRangeVelX(uint32_t *d0, uint32_t *d1, uint32_t *handler, const u16 **script, Vm4Obj *child,
                        dk::Flags *ccr)
{
    uint16_t r = (uint16_t)(uint32_t)Rand_Next();
    *d1 = (uint32_t)(int32_t)(int16_t)(uint16_t)*(*script)++ << 8;
    *d0 = (uint32_t)(((int32_t)(int16_t)(r >> 4) * (int16_t)(uint16_t)*(*script)++) >> 4);
    *d0 += *d1;
    uint32_t old = (uint32_t)(int32_t)child->vel_x;
    uint32_t sum = old + *d0;
    bool carry = sum < old;   /* add.l: X = C = the carry */
    *ccr = dk::Flags{ carry, (sum >> 31) != 0, sum == 0, ((~(old ^ *d0)) & (old ^ sum) & 0x80000000u) != 0, carry };
    child->vel_x = (int32_t)sum;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
