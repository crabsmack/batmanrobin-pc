#include "game.h"

/* Op2_SetVar2 / Op2_OrVar2: script variable 2 (a4 +$26) ($002180-$002191). */

/* Op2_SetVar2 - a2 op: sets script variable 2 to a word (natives read it: $1A04C, $25D80 adds
 * variable 3 to it, $26656 uses it as a y).
 * In:  script = word value
 * Out: vm->var[2]; NEXT
 * Evidence: move.w (a2)+,$26(a4) / NEXT; 2 corpus runs incl. the TAS
 */
void Op2_SetVar2(const u16 **script, Vm2Block *vm)
{
    vm->var[2] = *(*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_OrVar2 - a2 op: sets bits in script variable 2.
 * In:  script = word bits
 * Out: bits = the operand; vm->var[2] |= bits; NEXT
 * Evidence: move.w (a2)+,d0 / or.w d0,$26(a4) / NEXT; 1 corpus run (the TAS); prototype: a2 an
 *           output (its NEXT hands the advanced script to the next op; it was `->d0 clobbers(a2)`)
 */
void Op2_OrVar2(uint16_t *bits, const u16 **script, Vm2Block *vm)
{
    *bits = *(*script)++;
    vm->var[2] = (uint16_t)(vm->var[2] | *bits);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
