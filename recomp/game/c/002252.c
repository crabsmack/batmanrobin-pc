#include "game.h"

/* The set / bit ops on script variable 3 (a4 +$28) ($002252-$00226D). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SetVar3 - a2 op: sets script variable 3 to a word (the natives $25D6E/$25D80 add it to
 * variable 2 as a step).
 * In:  script = word value
 * Out: vm->var[3]; NEXT
 * Evidence: move.w (a2)+,$28(a4) / NEXT; 1 corpus run (the TAS, frame 33605)
 */
void Op2_SetVar3(const u16 **script, Vm2Block *vm)
{
    vm->var[3] = *(*script)++;
    next(script);
}

/* Op2_OrVar3 - a2 op: sets bits in script variable 3.
 * In:  script = word bits
 * Out: bits = the operand; vm->var[3] |= bits; NEXT
 * Evidence: move.w (a2)+,d0 / or.w d0,$28(a4) / NEXT; 11 corpus runs incl. the TAS
 */
void Op2_OrVar3(uint16_t *bits, const u16 **script, Vm2Block *vm)
{
    *bits = *(*script)++;
    vm->var[3] = (uint16_t)(vm->var[3] | *bits);
    next(script);
}

/* Op2_AndVar3 - a2 op: masks script variable 3 with a word.
 * In:  script = word mask
 * Out: mask = the operand; vm->var[3] &= mask; NEXT
 * Evidence: move.w (a2)+,d0 / and.w d0,$28(a4) / NEXT; 10 corpus runs incl. the TAS
 */
void Op2_AndVar3(uint16_t *mask, const u16 **script, Vm2Block *vm)
{
    *mask = *(*script)++;
    vm->var[3] = (uint16_t)(vm->var[3] & *mask);
    next(script);
}
