#include "game.h"

/* The cruise ops: the velocities and target speeds (a3 +$18/$1C, +$24/$2C) from the cruise
 * speeds (+$30/$34) ($0017EA-$001859). The steering tail ($4B8A) then moves each velocity toward
 * its target speed by the acceleration. */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_CruiseNow - a2 op: both velocities and both target speeds = the cruise speeds at once.
 * In:  mot->cruise_x / cruise_y
 * Out: mot->vel_y, vel_x, then as Op2_CruiseTargets
 * Evidence: move.l $34(a3),$1C(a3) / move.l $30(a3),$18(a3), then Op2_CruiseTargets' moves; 1
 *           corpus run (the TAS, frame 79014)
 */
void Op2_CruiseNow(const u16 **script, Vm2Motion *mot)
{
    mot->vel_y = mot->cruise_y;
    mot->vel_x = mot->cruise_x;
    Op2_CruiseTargets(script, mot);
}

/* Op2_CruiseTargets - a2 op: both target speeds = the cruise speeds (Op2_CruiseNow past its
 * velocity moves), so the steering accelerates toward them.
 * In:  mot->cruise_x / cruise_y
 * Out: mot->target_y, then as Op2_CruiseTargetX
 * Evidence: move.l $34(a3),$2C(a3); a computed-jump target in the corpus (~16k call-site entries)
 */
void Op2_CruiseTargets(const u16 **script, Vm2Motion *mot)
{
    mot->target_y = mot->cruise_y;
    Op2_CruiseTargetX(script, mot);
}

/* Op2_CruiseTargetX - a2 op: the x target speed = the x cruise speed (signed, unlike
 * Op2_CruiseRight/Left).
 * In:  mot->cruise_x
 * Out: mot->target_x; NEXT
 * Evidence: move.l $30(a3),$24(a3) / NEXT; a computed-jump target in the corpus (the NEXT of
 *           Op2_SetCruiseX)
 */
void Op2_CruiseTargetX(const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    mot->target_x = mot->cruise_x;
    next(&handler, script);
}

/* Op2_CruiseForward - a2 op: the x target speed = the x cruise speed's magnitude in the facing
 * direction: right unless the sprite is H-flipped.
 * In:  spr->attr bit 11 (the H flip, bit 3 of its high byte)
 * Out: as Op2_CruiseLeft (flipped) or Op2_CruiseRight
 * Evidence: btst #3,$38(a5) / bne.s Op2_CruiseLeft, falling into Op2_CruiseRight; 88 corpus runs
 *           incl. the TAS
 */
void Op2_CruiseForward(uint32_t *handler, const u16 **script, Vm2Motion *mot, const Vm2Sprite *spr)
{
    if (spr->attr & 0x0800) {
        Op2_CruiseLeft(handler, script, mot);
        return;
    }
    Op2_CruiseRight(handler, script, mot);
}

/* Op2_CruiseRight - a2 op: the x target speed = +|x cruise speed| (accelerate rightward); also
 * the 'target is to the right' exit of the X seek ops.
 * In:  mot->cruise_x
 * Out: mot->target_x; NEXT (through VM2_Next when the cruise speed was already >= 0)
 * Evidence: move.l $30(a3),$24(a3) / bpl.w VM2_Next / neg.l $24(a3) / NEXT; entered by dispatch
 *           in 73 runs
 */
void Op2_CruiseRight(uint32_t *handler, const u16 **script, Vm2Motion *mot)
{
    mot->target_x = mot->cruise_x;
    if (mot->target_x >= 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    mot->target_x = (int32_t)(0u - (uint32_t)mot->target_x);
    next(handler, script);
}

/* Op2_CruiseLeft - a2 op: the x target speed = -|x cruise speed| (accelerate leftward); also
 * the 'target is to the left' exit of the X seek ops.
 * In:  mot->cruise_x
 * Out: mot->target_x; NEXT (through VM2_Next when the cruise speed was already negative)
 * Evidence: move.l $30(a3),$24(a3) / bmi.w VM2_Next / neg.l $24(a3) / NEXT; entered by dispatch
 *           in 90 runs
 */
void Op2_CruiseLeft(uint32_t *handler, const u16 **script, Vm2Motion *mot)
{
    mot->target_x = mot->cruise_x;
    if (mot->target_x < 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    mot->target_x = (int32_t)(0u - (uint32_t)mot->target_x);
    next(handler, script);
}

/* Op2_CruiseRightNow - a2 op: the x target speed and the x velocity = +|x cruise speed| (moves
 * right at full speed at once).
 * In:  mot->cruise_x
 * Out: speed = |cruise_x|; mot->target_x / vel_x = speed; NEXT
 * Evidence: move.l $30(a3),d0 / bpl.s / neg.l d0 / move.l d0,$24(a3) / move.l d0,$18(a3); 1
 *           corpus run (the TAS)
 */
void Op2_CruiseRightNow(uint32_t *speed, const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    *speed = (uint32_t)mot->cruise_x;
    if ((int32_t)*speed < 0)
        *speed = 0u - *speed;
    mot->target_x = (int32_t)*speed;
    mot->vel_x = (int32_t)*speed;
    next(&handler, script);
}
