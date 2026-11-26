#include "game.h"

/* NEXT: the handler the script's next word names, jumped to */
static void op_next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SetTimer1Rand - a2 op: sets timer 1 to a random value below a range plus a base.
 * In:  script = word range, word base
 * Out: vm->timer[1] = Rand_Below(range) + base; rand = that value (d0's low word; its high word as
 *      Rand_Below leaves it); NEXT
 * Evidence: move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 / move.w d0,$1C(a4), Op2_SetTimer1
 *           with Op2_SetSpeedRand's operands; Scr2_0392A0 at $0392C2 (`0F88 0780 0780`); never run in the corpus.
 *           Prototype: Op2_SetSpeedRand's (the same operand reads)
 */
void Op2_SetTimer1Rand(uint32_t *rand, uint32_t *handler, const u16 **script, Vm2Block *vm)
{
    *rand = (*rand & 0xFFFF0000u) | *(*script)++;
    Rand_Below(rand);
    *rand = (*rand & 0xFFFF0000u) | (uint16_t)((uint16_t)*rand + *(*script)++);
    vm->timer[1] = (uint16_t)*rand;
    op_next(handler, script);
}
