#include "game.h"

/* Op4_JitterX - a4 op: moves the object's x by a random [-w/2, w/2) px: Rand_Next's signed low word
 * times w is a 16.16 offset added to the x long.
 * In:  script = word w
 * Out: self->x; NEXT
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / add.l d0,$2E(a6) / NEXT; script uses $26AA8, $3709E;
 *           42 corpus runs
 */
void Op4_JitterX(const u16 **script, Vm4Obj *self)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    uint32_t offset = (uint32_t)((int32_t)r * (int16_t)(uint16_t)*(*script)++);
    vm4_add_fix(self->x, self->x_frac, offset);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
