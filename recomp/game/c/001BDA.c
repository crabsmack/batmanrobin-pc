#include "game.h"

/* Op2_ClearVelY - a2 op: the y velocity = 0.
 * In:  -
 * Out: mot->vel_y = 0; NEXT
 * Evidence: clr.l $1C(a3) / NEXT; 2 corpus runs incl. the TAS
 */
void Op2_ClearVelY(const u16 **script, Vm2Motion *mot)
{
    mot->vel_y = 0;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
