#include "game.h"

/* Op2_LoopTimer1 - a2 op: timer 1 loses a tick; while it still runs the script branches (back);
 * when it runs out it is cleared and the script goes on. A timer already at 0 branches (the zero
 * test goes straight to the branch).
 * In:  script = a word offset; vm->timer[1], vm->tick
 * Out: at 0 already, or running: Op2_Goto; else timer cleared, NEXT after the offset
 * Evidence: tst.w $1C(a4) / beq.w Op2_Goto, then Op2_LoopTimer1NoZeroTest's code; executed in the TAS
 */
void Op2_LoopTimer1(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    if (vm->timer[1] == 0) {
        dk::jump(at_Op2_Goto);
        return;
    }
    Op2_LoopTimer1NoZeroTest(tick, handler, script, vm);
}

/* Op2_LoopTimer1NoZeroTest - a2 op: Op2_LoopTimer1 without its zero test: a timer at 0 borrows
 * and so falls through at once.
 * In:  script = a word offset; vm->timer[1], vm->tick
 * Out: tick = vm->tick; running: Op2_Goto; else timer cleared, NEXT after the offset
 * Evidence: move.w $30(a4),d0 / sub.w d0,$1C(a4) / bhi.w Op2_Goto / clr.w / addq.l #2,a2 / NEXT;
 *           a computed-jump target in the corpus
 */
void Op2_LoopTimer1NoZeroTest(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *tick = vm->tick;
    uint16_t t = vm->timer[1];
    vm->timer[1] = (uint16_t)(t - *tick);
    if (t > *tick) {
        dk::jump(at_Op2_Goto);
        return;
    }
    vm->timer[1] = 0;
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
