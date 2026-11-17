#include "game.h"

/* Op4_SpawnHeadingFromSelf / Op4_SpawnXYFromSelf / Op4_ClearSpawnPolar ($0063B6-$0063D1): the
 * spawn registers (d2 radius, d3 heading, d4 / d5 x / y, 16.16) the next spawn starts from. */

static void next(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op4_SpawnHeadingFromSelf - a4 op: sets the spawn heading register to the object's heading and
 * the spawn radius to 0.
 * In:  script (no operands); self = a6
 * Out: d3 = self's heading (16.16); d2 = 0; NEXT
 * Evidence: move.l $2A(a6),d3 / moveq #0,d2 / NEXT; script uses $42994, $46AFA; 43 corpus runs
 */
void Op4_SpawnHeadingFromSelf(uint32_t *d2, uint32_t *d3, const u16 **script, const Vm4Obj *self)
{
    *d3 = vm4_fix(self->heading, self->heading_frac);
    *d2 = 0;
    next(script);
}

/* Op4_SpawnXYFromSelf - a4 op: sets the spawn position registers to the object's x / y.
 * In:  script (no operands); self = a6
 * Out: d4 / d5 = x / y (16.16); NEXT
 * Evidence: movem.l $2E(a6),d4-d5 / NEXT; script uses $35A00, $37022 (`63C0 63CA 607C`); 31
 *           corpus runs
 */
void Op4_SpawnXYFromSelf(uint32_t *d4, uint32_t *d5, const u16 **script, const Vm4Obj *self)
{
    *d4 = vm4_fix(self->x, self->x_frac);
    *d5 = vm4_fix(self->y, self->y_frac);
    next(script);
}

/* Op4_ClearSpawnPolar - a4 op: clears the spawn radius and heading registers.
 * In:  script (no operands)
 * Out: d2 = d3 = 0; NEXT
 * Evidence: moveq #0,d2 / moveq #0,d3 / NEXT; script uses $1E384, $37024; 29 corpus runs
 */
void Op4_ClearSpawnPolar(uint32_t *d2, uint32_t *d3, const u16 **script)
{
    *d2 = 0;
    *d3 = 0;
    next(script);
}
