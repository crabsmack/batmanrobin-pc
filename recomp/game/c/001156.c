#include "game.h"

/* Op2_TickTimer3Branch - a2 op: timer 3 loses a tick; branches when it runs out this run. Unlike
 * timers 0-2's op it has no zero test: a timer at 0 borrows and so branches too.
 * In:  script = a word offset; vm->timer[3], vm->tick
 * Out: tick = vm->tick; still running: Op2_SkipWord; else timer cleared, the branch by the offset
 * Evidence: move.w $30(a4),d0 / sub.w d0,$20(a4) / bhi.w Op2_SkipWord / clr.w $20(a4) /
 *           adda.w (a2),a2 / NEXT; executed in the TAS
 */
void Op2_TickTimer3Branch(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *tick = vm->tick;
    uint16_t t = vm->timer[3];
    vm->timer[3] = (uint16_t)(t - *tick);
    if (t > *tick) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    vm->timer[3] = 0;
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
