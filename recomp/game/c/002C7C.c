#include "game.h"

/* Op2_LoopCounter1 and Op2_SetCounter2 ($002C7C-$002C91; the counter 2 random twin at $2C92 is
 * raw bytes, never run). */

/* subq.w #1 then bgt: the counter's new value, and whether the true difference is above 0 */
static bool count_down(u16 &counter)
{
    int32_t left = (int32_t)(int16_t)(uint16_t)counter - 1;
    counter = (uint16_t)left;
    return left > 0;
}

/* Op2_LoopCounter1 - a2 op: decrements loop counter 1 and branches while it is still above 0.
 * In:  script = word offset; vm->counter[1]
 * Out: vm->counter[1] - 1; above 0: Op2_Goto (the branch); else NEXT after the offset
 * Evidence: subq.w #1,$16(a4) / bgt.w Op2_Goto / addq.l #2,a2 / NEXT; 32 runs
 */
void Op2_LoopCounter1(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    if (count_down(vm->counter[1])) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SetCounter2 - a2 op: sets loop counter 2 for Op2_LoopCounter2.
 * In:  script = word count
 * Out: vm->counter[2]; NEXT
 * Evidence: move.w (a2)+,$18(a4) / NEXT; 15 runs
 */
void Op2_SetCounter2(const u16 **script, Vm2Block *vm)
{
    vm->counter[2] = *(*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
