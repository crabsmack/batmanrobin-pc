#include "game.h"

/* Op4_JitterHeading - a4 op: turns the object's heading by a random [-w/2, w/2) units ($400 a turn):
 * Rand_Next's signed low word times w added to the heading long.
 * In:  script = word w
 * Out: d0 = the offset; self->heading; NEXT
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / add.l d0,$2A(a6) / NEXT; script uses $26AB0, $2981A,
 *           $3601E; 33 corpus runs
 */
void Op4_JitterHeading(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    *d0 = (uint32_t)((int32_t)r * (int16_t)(uint16_t)*(*script)++);
    vm4_add_fix(self->heading, self->heading_frac, *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
