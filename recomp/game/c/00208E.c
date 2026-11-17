#include "game.h"

/* Op2_SwitchVar0Bits and the set / bit ops on script variable 1 (a4 +$24) ($00208E-$0020F1). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SwitchVar0Bits - a2 op: bit switch on script variable 0: walks n (offset, mask) word
 * pairs and branches by the offset of the first pair whose mask shares a bit with the variable
 * (the offset counts from the end of its pair); no match continues after the table.
 * In:  script = word n-1, then n (word offset, word mask) pairs; vm->var[0]
 * Out: value = var0; pair = the last pair read (offset:mask&var0), swapped on a match;
 *      count = the dbne counter as it stopped ($FFFF when no pair matched); the case, or NEXT
 *      after the table
 * Evidence: move.w $22(a4),d0 / move.w (a2)+,d7 / move.l (a2)+,d1 / and.w d0,d1 / dbne d7 /
 *           beq.s / swap d1 / adda.w d1,a2; 21 corpus runs incl. the TAS (12532 TAS frames)
 */
void Op2_SwitchVar0Bits(uint16_t *value, uint32_t *pair, uint16_t *count, const u16 **script, Vm2Block *vm)
{
    *value = vm->var[0];
    *count = *(*script)++;
    uint16_t hit;
    for (;;) {
        uint16_t offset = (*script)[0];
        hit = (uint16_t)((*script)[1] & *value);
        *script += 2;
        *pair = (uint32_t)offset << 16 | hit;
        if (hit != 0)
            break;
        *count = (uint16_t)(*count - 1);
        if (*count == 0xFFFF)
            break;
    }
    if (hit != 0) {
        *pair = *pair << 16 | *pair >> 16;
        *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)*pair);
    }
    next(script);
}

/* Op2_ClearVar1 - a2 op: clears script variable 1 (a flag word: bit $400 tested by the player
 * code $10322).
 * In:  -
 * Out: vm->var[1] = 0; NEXT
 * Evidence: clr.w $24(a4) / NEXT; 36 corpus runs incl. the TAS
 */
void Op2_ClearVar1(const u16 **script, Vm2Block *vm)
{
    vm->var[1] = 0;
    next(script);
}

/* Op2_SetVar1 - a2 op: sets script variable 1 to a word.
 * In:  script = word value
 * Out: vm->var[1]; NEXT
 * Evidence: move.w (a2)+,$24(a4) / NEXT; 9 corpus runs incl. the TAS
 */
void Op2_SetVar1(const u16 **script, Vm2Block *vm)
{
    vm->var[1] = *(*script)++;
    next(script);
}

/* Op2_OrVar1 - a2 op: sets bits in script variable 1.
 * In:  script = word bits
 * Out: bits = the operand; vm->var[1] |= bits; NEXT
 * Evidence: move.w (a2)+,d0 / or.w d0,$24(a4) / NEXT; 135 corpus runs incl. the TAS
 */
void Op2_OrVar1(uint16_t *bits, const u16 **script, Vm2Block *vm)
{
    *bits = *(*script)++;
    vm->var[1] = (uint16_t)(vm->var[1] | *bits);
    next(script);
}

/* Op2_AndVar1 - a2 op: masks script variable 1 with a word (clears the bits not in it).
 * In:  script = word mask
 * Out: mask = the operand; vm->var[1] &= mask; handler = the next op's; NEXT
 * Evidence: move.w (a2)+,d0 / and.w d0,$24(a4) / NEXT; 144 corpus runs incl. the TAS
 */
void Op2_AndVar1(uint16_t *mask, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *mask = *(*script)++;
    vm->var[1] = (uint16_t)(vm->var[1] & *mask);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_XorVar1 - a2 op: toggles bits in script variable 1.
 * In:  script = word bits
 * Out: bits = the operand; vm->var[1] ^= bits; NEXT
 * Evidence: move.w (a2)+,d0 / eor.w d0,$24(a4) / NEXT; 20 corpus runs incl. the TAS
 */
void Op2_XorVar1(uint16_t *bits, const u16 **script, Vm2Block *vm)
{
    *bits = *(*script)++;
    vm->var[1] = (uint16_t)(vm->var[1] ^ *bits);
    next(script);
}

/* Op2_IncVar1 - a2 op: adds 1 to script variable 1.
 * In:  vm->var[1]
 * Out: vm->var[1] + 1; NEXT
 * Evidence: addq.w #1,$24(a4) / NEXT; 4 corpus runs incl. the TAS; prototype: a2 an output (its
 *           NEXT hands the advanced script to the next op; it was `clobbers(a2)`)
 */
void Op2_IncVar1(const u16 **script, Vm2Block *vm)
{
    vm->var[1] = (uint16_t)(vm->var[1] + 1);
    next(script);
}
