#include "game.h"

/* Op2_SwitchVar2Bits - a2 op: the bit switch of Op2_SwitchVar0Bits on script variable 2 (a4
 * +$26): branches by the offset of the first (offset, mask) pair whose mask shares a bit with
 * the variable (the offset counts from the end of its pair); no match continues after the table.
 * In:  script = word n-1, then n (word offset, word mask) pairs; vm->var[2]
 * Out: value = var2; pair = the last pair read (offset:mask&var2), swapped on a match;
 *      count = the dbne counter as it stopped ($FFFF when no pair matched); the case, or NEXT
 *      after the table
 * Evidence: the loop of $208E with $26(a4): move.l (a2)+,d1 / and.w d0,d1 / dbne d7 / beq.s /
 *           swap d1 / adda.w d1,a2; 21 corpus runs incl. the TAS
 */
void Op2_SwitchVar2Bits(uint16_t *value, uint32_t *pair, uint16_t *count, const u16 **script, Vm2Block *vm)
{
    *value = vm->var[2];
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
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
