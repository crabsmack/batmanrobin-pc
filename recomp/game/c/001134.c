#include "game.h"

/* Timer 3's set ops ($001134-$001155). */

/* Op2_SetTimer3 - a2 op: timer 3 = the operand.
 * In:  script = word ticks
 * Out: vm->timer[3]; NEXT
 * Evidence: move.w (a2)+,$20(a4) / NEXT; executed in the TAS
 */
void Op2_SetTimer3(const u16 **script, Vm2Block *vm)
{
    vm->timer[3] = *(*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SetTimer3Rand - a2 op: timer 3 = base + a random value below range (Rand_Below).
 * In:  script = words range, base
 * Out: vm->timer[3]; rand's low word = the ticks (its upper word Rand_Below's); NEXT
 * Evidence: move.w (a2)+,d0 / jsr Rand_Below / add.w (a2)+,d0 / move.w d0,$20(a4) / NEXT
 */
void Op2_SetTimer3Rand(uint32_t *rand, const u16 **script, Vm2Block *vm)
{
    *rand = (*rand & 0xFFFF0000u) | *(*script)++;
    Rand_Below(rand);
    *rand = (*rand & 0xFFFF0000u) | (uint16_t)(*rand + *(*script)++);
    vm->timer[3] = (uint16_t)*rand;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
