#include "game.h"

/* Loop counters 0 and 1 (a4 +$14/$16): Op2_SetCounter0, Op2_SetCounter0Rand, Op2_LoopCounter0 and
 * Op2_SetCounter1 ($002C3A-$002C69). Unlike the timers, a counter loses 1 per run of its loop op,
 * not the tick. (The counter 1 random twin at $2C6A is raw bytes, never run.) */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SetCounter0 - a2 op: sets loop counter 0 for Op2_LoopCounter0.
 * In:  script = word count
 * Out: vm->counter[0]; NEXT
 * Evidence: move.w (a2)+,$14(a4) / NEXT; 109 runs; e.g. $1377E `2C3A 0005 ... 2C54 FFF0`
 */
void Op2_SetCounter0(const u16 **script, Vm2Block *vm)
{
    vm->counter[0] = *(*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SetCounter0Rand - a2 op: sets loop counter 0 to base + a random value below range.
 * In:  script = words base, range
 * Out: base (d1's low word) = the base; vm->counter[0]; NEXT
 * Evidence: move.w (a2)+,d1 / move.w (a2)+,d0 / bsr Rand_Below / add.w d1,d0 / move.w d0,$14(a4);
 *           9 runs; $38B52 `2C42 0003 0002` = 3..4
 */
void Op2_SetCounter0Rand(uint16_t *base, const u16 **script, Vm2Block *vm)
{
    *base = *(*script)++;
    uint32_t value = *(*script)++;
    Rand_Below(&value);
    vm->counter[0] = (uint16_t)(value + *base);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* subq.w #1 then bgt: the counter's new value, and whether the true difference is above 0 (bgt
   after a subtraction compares the exact result, so $8000 - 1 is not "greater") */
static bool count_down(u16 &counter)
{
    int32_t left = (int32_t)(int16_t)(uint16_t)counter - 1;
    counter = (uint16_t)left;
    return left > 0;
}

/* Op2_LoopCounter0 - a2 op: decrements loop counter 0 and branches (a counted loop back) while it
 * is still above 0.
 * In:  script = word offset; vm->counter[0]
 * Out: vm->counter[0] - 1; above 0: Op2_Goto (the branch); else NEXT after the offset
 * Evidence: subq.w #1,$14(a4) / bgt.w Op2_Goto / addq.l #2,a2 / NEXT; 116 runs
 */
void Op2_LoopCounter0(uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    if (count_down(vm->counter[0])) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(handler, script);
}

/* Op2_SetCounter1 - a2 op: sets loop counter 1 for Op2_LoopCounter1.
 * In:  script = word count
 * Out: vm->counter[1]; NEXT
 * Evidence: move.w (a2)+,$16(a4) / NEXT; 38 runs; $255DC `2C62 0003`
 */
void Op2_SetCounter1(const u16 **script, Vm2Block *vm)
{
    vm->counter[1] = *(*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
