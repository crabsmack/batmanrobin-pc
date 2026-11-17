#include "game.h"

/* Timer 2's set and tick-and-branch ops ($00105A-$0010E1). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SetTimer2 - a2 op: timer 2 = the operand.
 * In:  script = word ticks
 * Out: vm->timer[2]; NEXT
 * Evidence: move.w (a2)+,$1E(a4) / NEXT; executed in the TAS
 */
void Op2_SetTimer2(const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    vm->timer[2] = *(*script)++;
    next(&handler, script);
}

/* Op2_SetTimer2Rand - a2 op: timer 2 = base + a random value below range (Rand_Below).
 * In:  script = words range, base
 * Out: vm->timer[2]; rand's low word = the ticks (its upper word Rand_Below's); NEXT
 * Evidence: move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 / move.w d0,$1E(a4) / NEXT
 */
void Op2_SetTimer2Rand(uint32_t *rand, const u16 **script, Vm2Block *vm)
{
    uint32_t handler;
    *rand = (*rand & 0xFFFF0000u) | *(*script)++;
    Rand_Below(rand);
    *rand = (*rand & 0xFFFF0000u) | (uint16_t)(*rand + *(*script)++);
    vm->timer[2] = (uint16_t)*rand;
    next(&handler, script);
}

/* Op2_TickTimer2Branch - a2 op: timer 2 loses a tick; branches when it runs out this run. A timer
 * already at 0 does not tick and does not branch.
 * In:  script = a word offset; vm->timer[2], vm->tick
 * Out: at 0 already, or still running: Op2_SkipWord; expired: timer cleared, the branch
 * Evidence: tst.w $1E(a4) / beq.w Op2_SkipWord, then Op2_TickTimer2BranchNoZeroTest's code
 */
void Op2_TickTimer2Branch(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    if (vm->timer[2] == 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    Op2_TickTimer2BranchNoZeroTest(tick, handler, script, vm);
}

/* Op2_TickTimer2BranchNoZeroTest - a2 op: Op2_TickTimer2Branch without its zero test: a timer at
 * 0 borrows and so branches too.
 * In:  script = a word offset; vm->timer[2], vm->tick
 * Out: tick = vm->tick; still running: Op2_SkipWord; else timer cleared, the branch by the offset
 * Evidence: move.w $30(a4),d0 / sub.w d0,$1E(a4) / bhi.w Op2_SkipWord / clr.w / adda.w (a2),a2 /
 *           NEXT; a computed-jump target in the corpus
 */
void Op2_TickTimer2BranchNoZeroTest(uint16_t *tick, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *tick = vm->tick;
    uint16_t t = vm->timer[2];
    vm->timer[2] = (uint16_t)(t - *tick);
    if (t > *tick) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    vm->timer[2] = 0;
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}
