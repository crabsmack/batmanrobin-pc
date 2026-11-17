#include "game.h"

/* Op2_CruiseToPlayerX ($002F7C-$002F9B). */

/* Op2_CruiseToPlayerX - a2 op: cruises toward the targeted player's x without changing the
 * facing: x target speed = +cruise when the player is at or right of the object, else
 * Op2_CruiseLeft; with no target, Op2_ClearTargetSpeedX (brakes x).
 * In:  vm->tracked = the targeted player record (0 = none); mot->cruise_x; self->x
 * Out: d0's low word = the record handle, then the player's x; a0 = the record; mot->target_x;
 *      NEXT (or the two tails)
 * Evidence: move.w $10(a4),d0 / beq.w Op2_ClearTargetSpeedX / movea.w d0,a0 / move.w $2E(a0),d0 /
 *           cmp.w $3E(a6),d0 / blt.w Op2_CruiseLeft / move.l $30(a3),$24(a3) / NEXT; 43 runs
 */
void Op2_CruiseToPlayerX(uint16_t *d0, uint32_t *a0, const u16 **script, Vm2Motion *mot, const Vm2Block *vm,
                         const Vm2Obj *self)
{
    *d0 = (uint16_t)vm->tracked;
    if (*d0 == 0) {
        dk::jump(at_Op2_ClearTargetSpeedX);
        return;
    }
    *a0 = (uint32_t)(int32_t)vm->tracked;
    const PlayerRec *target = dk::handle<PlayerRec>(vm->tracked);
    *d0 = (uint16_t)target->x;
    if ((int16_t)*d0 < self->x) {
        dk::jump(at_Op2_CruiseLeft);
        return;
    }
    mot->target_x = mot->cruise_x;
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}
