#include "game.h"

/* Op4_ChildJitterX - a4 op: moves the last spawned child's x by a random [-w/2, w/2) px: Rand_Next's
 * signed low word times w, added to the x long.
 * In:  script = word w; child = a5
 * Out: d0 = the offset (16.16); child->x; the add.l's condition codes; NEXT (handler = the next op)
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / add.l d0,$2E(a5) / NEXT; script uses $1A390, $1ECAC,
 *           $1ECE4; 7 corpus runs
 */
void Op4_ChildJitterX(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    *d0 = (uint32_t)((int32_t)r * (int16_t)(uint16_t)*(*script)++);
    uint32_t old = vm4_fix(child->x, child->x_frac);
    uint32_t sum = old + *d0;
    bool carry = sum < old;
    *ccr = dk::Flags{ carry, (sum >> 31) != 0, sum == 0, ((~(old ^ *d0)) & (old ^ sum) & 0x80000000u) != 0, carry };
    vm4_set_fix(child->x, child->x_frac, sum);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
