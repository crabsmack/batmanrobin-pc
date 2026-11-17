#include "game.h"

/* Op4_ChildJitterXY / Op4_ChildJitterY ($005D1A-$005D33): random moves of the last spawned
 * child's position. */

/* Rand_Next's signed low word times the operand w (muls.w (a4)+,d0): [-w/2, w/2) px as 16.16 */
static uint32_t jitter(const u16 **script)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    return (uint32_t)((int32_t)r * (int16_t)(uint16_t)*(*script)++);
}

/* Op4_ChildJitterXY - a4 op: moves the child's x and y by random [-wx/2, wx/2) / [-wy/2, wy/2) px.
 * In:  script = word wx, word wy; child = a5
 * Out: child->x; then as Op4_ChildJitterY
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / add.l d0,$2E(a5), falling into Op4_ChildJitterY; 26
 *           a4-script uses, e.g. $1A632, $1A64A (after a spawn + Op4_ChildSaveDrawPos); 6 corpus runs
 */
void Op4_ChildJitterXY(const u16 **script, Vm4Obj *child)
{
    vm4_add_fix(child->x, child->x_frac, jitter(script));
    Op4_ChildJitterY(script, child);
}

/* Op4_ChildJitterY - a4 op: moves the child's y by a random [-w/2, w/2) px.
 * In:  script = word w; child = a5
 * Out: child->y; NEXT
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / add.l d0,$32(a5) / NEXT; a label inside
 *           Op4_ChildJitterXY, a computed-jump target in 1 corpus run (fuzz); script use $4954A
 */
void Op4_ChildJitterY(const u16 **script, Vm4Obj *child)
{
    vm4_add_fix(child->y, child->y_frac, jitter(script));
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
