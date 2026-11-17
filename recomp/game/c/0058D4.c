#include "game.h"

/* Op4_ChildAddXY - a4 op: adds signed pixel words to the integer words of the last spawned
 * child's x and y: the x here, then the code falls into Op4_ChildAddY (the next file) for the y.
 * In:  script = word dx, word dy; child = a5
 * Out: d0 = dx; child->x; then Op4_ChildAddY
 * Evidence: move.w (a4)+,d0 / add.w d0,$2E(a5) with no NEXT before $58DA; script use $49544;
 *           1 corpus run
 */
void Op4_ChildAddXY(uint16_t *d0, const u16 **script, Vm4Obj *child)
{
    *d0 = *(*script)++;
    child->x = (int16_t)(uint16_t)((uint16_t)child->x + *d0);
    dk::jump(at_Op4_ChildAddY);
}
