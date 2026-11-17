#include "game.h"

/* Op4_ChildAddY - a4 op: adds a signed pixel word to the integer word of the last spawned child's y.
 * In:  script = word py; child = a5
 * Out: d0 = it; child->y; NEXT
 * Evidence: move.w (a4)+,d0 / add.w d0,$32(a5) / NEXT; script uses $1F270, $25AFA; also the tail of
 *           Op4_ChildAddXY; 20 corpus runs
 */
void Op4_ChildAddY(uint16_t *d0, const u16 **script, Vm4Obj *child)
{
    *d0 = *(*script)++;
    child->y = (int16_t)(uint16_t)((uint16_t)child->y + *d0);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
