#include "game.h"

/* Op4_JitterXY / Op4_JitterY ($005990-$0059A9): random moves of the object's position. */

/* Rand_Next's signed low word times the operand w (muls.w (a4)+,d0): [-w/2, w/2) px as 16.16 */
static uint32_t jitter(const u16 **script)
{
    int16_t r = (int16_t)(uint16_t)(uint32_t)Rand_Next();
    return (uint32_t)((int32_t)r * (int16_t)(uint16_t)*(*script)++);
}

/* Op4_JitterXY - a4 op: moves the object's x and y by random [-wx/2, wx/2) / [-wy/2, wy/2) px.
 * In:  script = word wx, word wy
 * Out: self->x; then as Op4_JitterY
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / add.l d0,$2E(a6), falling into Op4_JitterY; script uses
 *           $35A48, $46956, $4697E; 3 corpus runs
 */
void Op4_JitterXY(const u16 **script, Vm4Obj *self)
{
    vm4_add_fix(self->x, self->x_frac, jitter(script));
    Op4_JitterY(script, self);
}

/* Op4_JitterY - a4 op: moves the object's y by a random [-w/2, w/2) px.
 * In:  script = word w
 * Out: self->y; NEXT
 * Evidence: bsr Rand_Next / muls.w (a4)+,d0 / add.l d0,$32(a6) / NEXT; a label inside Op4_JitterXY,
 *           a computed-jump target in 29 corpus runs; script use $370A2 right after Op4_JitterX
 */
void Op4_JitterY(const u16 **script, Vm4Obj *self)
{
    vm4_add_fix(self->y, self->y_frac, jitter(script));
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
