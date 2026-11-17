#include "game.h"

/* Op2_FxMarkPlayerTarget - a2 op: stores the targeted player's position (the record the VM block's
 * tracked handle names) in g_ArcTargetX/Y, for the effect script spawned next (its Arc_PlanToMark
 * aims there); with no target skips the next 4 script bytes (that spawn op and its operand).
 * In:  vm (tracked: a PlayerRec handle or 0)
 * Out: g_ArcTargetX/Y = the record's x/y words; NEXT, or NEXT 4 bytes on
 * Evidence: move.w $10(a4),d0 / beq / movea.w d0,a0 / move.w $2E(a0),(g_ArcTargetX).w / move.w $32(a0),
 *           (g_ArcTargetY).w / NEXT; else addq.l #4,a2 / NEXT; followed by $5198 + word in its script
 *           uses ($1B446, $1F340, $298C2, $42C98); 16 corpus runs, 23 TAS frames
 */
void Op2_FxMarkPlayerTarget(const u16 **script, const Vm2Block *vm)
{
    if (vm->tracked != 0) {
        const PlayerRec *target = dk::handle<PlayerRec>(vm->tracked);
        g_ArcTargetX = (uint16_t)target->x;
        g_ArcTargetY = (uint16_t)target->y;
    } else {
        *script += 2;
    }
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
