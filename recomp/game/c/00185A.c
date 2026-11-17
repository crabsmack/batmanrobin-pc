#include "game.h"

/* The braking ops: the target speeds (a3 +$24 x, +$2C y) cleared, so the steering tail brakes
 * the velocity to a stop by the acceleration ($00185A-$001871). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_StopCruiseX - a2 op: clears the x cruise speed, then the x target speed.
 * In:  -
 * Out: mot->cruise_x = 0, then as Op2_ClearTargetSpeedX
 * Evidence: clr.l $30(a3), falling into Op2_ClearTargetSpeedX; 89 corpus runs incl. the TAS
 */
void Op2_StopCruiseX(const u16 **script, Vm2Motion *mot)
{
    mot->cruise_x = 0;
    Op2_ClearTargetSpeedX(script, mot);
}

/* Op2_ClearTargetSpeedX - a2 op: the x target speed = 0 (the x velocity brakes to a stop).
 * In:  -
 * Out: mot->target_x = 0; NEXT
 * Evidence: clr.l $24(a3) / NEXT; $4B8A moves $18 toward $24 by $20; 152 corpus runs incl. the TAS
 */
void Op2_ClearTargetSpeedX(const u16 **script, Vm2Motion *mot)
{
    mot->target_x = 0;
    next(script);
}

/* Op2_ClearTargetSpeeds - a2 op: both target speeds = 0, braking in both axes.
 * In:  -
 * Out: mot->target_y = mot->target_x = 0; NEXT
 * Evidence: clr.l $2C(a3) / clr.l $24(a3) / NEXT; 17 corpus runs incl. the TAS
 */
void Op2_ClearTargetSpeeds(const u16 **script, Vm2Motion *mot)
{
    mot->target_y = 0;
    mot->target_x = 0;
    next(script);
}
