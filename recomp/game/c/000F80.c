#include "game.h"

/* Op2_SetTimer1 - a2 op: timer 1 = the operand (counted down by the VM block's tick per run).
 * In:  script = word ticks
 * Out: vm->timer[1]; NEXT
 * Evidence: move.w (a2)+,$1C(a4) / NEXT; counted down by Op2_TickTimer1Branch / Op2_LoopTimer1;
 *           executed in the TAS
 */
void Op2_SetTimer1(const u16 **script, Vm2Block *vm)
{
    vm->timer[1] = *(*script)++;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
