#include "game.h"

/* Op2_LoopTimer2 - a2 op: timer 2 loses a tick (the VM block's tick); while it still runs the
 * script branches (back, a wait loop); when it runs out it is cleared and the script goes on. A
 * timer already at 0 borrows and so falls through at once.
 * In:  script = a word offset; vm->timer[2], vm->tick
 * Out: tick = vm->tick; running: Op2_Goto (the branch); else timer cleared, NEXT after the offset
 * Evidence: move.w $30(a4),d0 / sub.w d0,$1E(a4) / bhi.w Op2_Goto / clr.w $1E(a4) / addq.l #2,a2 /
 *           NEXT; executed in the TAS
 */
void Op2_LoopTimer2(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *tick = vm->tick;
    uint16_t t = vm->timer[2];
    vm->timer[2] = (uint16_t)(t - *tick);
    if (t > *tick) {
        dk::jump(at_Op2_Goto);
        return;
    }
    vm->timer[2] = 0;
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
