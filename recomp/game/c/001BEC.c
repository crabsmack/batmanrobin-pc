#include "game.h"

/* Op2_CruiseDown / Op2_CruiseUp: the y target speed (a3 +$2C) from the y cruise speed's
 * magnitude ($001BEC-$001C0F); also the Y seek ops' 'target below / above' exits. */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_CruiseDown - a2 op: the y target speed = +|y cruise speed| (accelerate downward).
 * In:  mot->cruise_y
 * Out: mot->target_y; NEXT (through VM2_Next when the cruise speed was already >= 0)
 * Evidence: move.l $34(a3),$2C(a3) / bpl.w VM2_Next / neg.l $2C(a3) / NEXT; branched to from
 *           the Y seek ops $1A62-$1B70; entered by dispatch in 83 runs
 */
void Op2_CruiseDown(uint32_t *handler, const u16 **script, Vm2Motion *mot)
{
    mot->target_y = mot->cruise_y;
    if (mot->target_y >= 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    mot->target_y = (int32_t)(0u - (uint32_t)mot->target_y);
    next(handler, script);
}

/* Op2_CruiseUp - a2 op: the y target speed = -|y cruise speed| (accelerate upward).
 * In:  mot->cruise_y
 * Out: mot->target_y; NEXT (through VM2_Next when the cruise speed was already negative)
 * Evidence: move.l $34(a3),$2C(a3) / bmi.w VM2_Next / neg.l $2C(a3) / NEXT; entered by dispatch
 *           in 75 runs
 */
void Op2_CruiseUp(uint32_t *handler, const u16 **script, Vm2Motion *mot)
{
    mot->target_y = mot->cruise_y;
    if (mot->target_y < 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    mot->target_y = (int32_t)(0u - (uint32_t)mot->target_y);
    next(handler, script);
}
