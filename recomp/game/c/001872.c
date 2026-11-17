#include "game.h"

/* Op2_BrakeXGotoIfStopped - a2 op: the x target speed = 0 (brake), then branches when the x
 * velocity is already 0, else continues after the branch offset.
 * In:  script = word branch offset; mot->vel_x
 * Out: mot->target_x = 0; the branch (Op2_Goto) when stopped, else NEXT after the offset
 * Evidence: clr.l $24(a3) / tst.l $18(a3) / beq.w Op2_Goto / addq.l #2,a2 + NEXT; run only on
 *           the stepped runner (P4 kickoff 2026-10-27, 5 fuzz runs)
 */
void Op2_BrakeXGotoIfStopped(uint32_t *handler, const u16 **script, Vm2Motion *mot)
{
    mot->target_x = 0;
    if (mot->vel_x == 0) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
