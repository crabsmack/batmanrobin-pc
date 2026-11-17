#include "game.h"

/* Op4_ChildAddX - a4 op: adds a signed pixel word to the integer word of the last spawned child's x.
 * In:  script = word px; child = a5
 * Out: child->x; NEXT
 * Evidence: move.w (a4)+,d0 / add.w d0,$2E(a5) / NEXT; script uses $26C62, $26C76 (after a spawn);
 *           3 corpus runs (fuzz only)
 */
void Op4_ChildAddX(const u16 **script, Vm4Obj *child)
{
    uint16_t px = *(*script)++;
    child->x = (int16_t)(uint16_t)((uint16_t)child->x + px);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
