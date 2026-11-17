#include "game.h"

/* $004B74-$004C61: this frame's move. Op2_StepVelocity steers the motion block's velocity toward
 * its target speeds, adds the decaying impulses, and leaves the move (16.16) in d1 / d2 for the
 * next op: Op2_MoveTerrain, Op2_ApplyMove or the clamps that keep the object in the movement area. */

/* NEXT: the handler the script's next word names, jumped to */
static void op_next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the object's 16.16 position (x at +$3E, y at +$42) */
static uint32_t pos_x(const Vm2Obj *o) { return (uint32_t)(uint16_t)o->x << 16 | o->x_frac; }
static uint32_t pos_y(const Vm2Obj *o) { return (uint32_t)(uint16_t)o->y << 16 | o->y_frac; }
static void set_pos_x(Vm2Obj *o, uint32_t v) { o->x = (int16_t)(v >> 16); o->x_frac = (uint16_t)v; }
static void set_pos_y(Vm2Obj *o, uint32_t v) { o->y = (int16_t)(v >> 16); o->y_frac = (uint16_t)v; }

/* the movement area's size as the 16.16 longs the clamps subtract (sub.l (g_MoveAreaW).w: the
 * width word and the word after it, its fraction) */
static uint32_t area_w(void) { return *(const u32 *)&g_MoveAreaW; }
static uint32_t area_h(void) { return *(const u32 *)&g_MoveAreaH; }

/* one axis of the steering: v moves toward target by step, landing on it rather than passing it */
static uint32_t steer(uint32_t v, uint32_t step, uint32_t target)
{
    if ((int32_t)v < (int32_t)target) {
        v += step;
        return (int32_t)v < (int32_t)target ? v : target;
    }
    v -= step;
    return (int32_t)v > (int32_t)target ? v : target;
}

/* one impulse decayed by $1400 toward 0 (stopping at 0) */
static uint32_t decay(uint32_t impulse)
{
    if ((int32_t)impulse < 0) {
        impulse += 0x1400;
        return (int32_t)impulse < 0 ? impulse : 0;
    }
    impulse -= 0x1400;
    return (int32_t)impulse >= 0 ? impulse : 0;
}

/* Op2_StepVelocity - a2 op: steps the motion block's velocity for this frame and leaves the move in
 * d1 / d2: each axis moves toward its target speed by its acceleration, then the decaying impulses
 * are added. While the stun count runs the velocity part is 0.
 * In:  mot: vel_x/y, accel_x/y, target_x/y, impulse_x/y, stun
 * Out: d1 / d2 = the x / y move (16.16); d3-d6 = the accelerations and targets (not stunned);
 *      d0 = the y impulse; NEXT (VM2_Next with no y impulse)
 * Evidence: tst.b $1(a3) / subq.b #1 / moveq #0,d1 / moveq #0,d2 / bra.s Op2_StepVelocity_Impulse;
 *           movem.l $18(a3),d1-d6, falls into Op2_StepVelocity_Steer; every object loop (125k TAS
 *           frames, all 160 runs), e.g. $11B84 `4B74 4C52`
 */
void Op2_StepVelocity(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *a0, const u16 **script, Vm2Motion *mot)
{
    if (mot->stun != 0) {
        mot->stun = (uint8_t)(mot->stun - 1);
        *d1 = 0;
        *d2 = 0;
        Op2_StepVelocity_Impulse(d0, d1, d2, a0, script, mot);
        return;
    }
    *d1 = (uint32_t)mot->vel_x;
    *d2 = (uint32_t)mot->vel_y;
    *d3 = (uint32_t)mot->accel_x;
    *d4 = (uint32_t)mot->target_x;
    *d5 = (uint32_t)mot->accel_y;
    *d6 = (uint32_t)mot->target_y;
    Op2_StepVelocity_Steer(d0, d1, d2, *d3, *d4, *d5, *d6, a0, script, mot);
}

/* Op2_StepVelocity_Steer - the steering tail of Op2_StepVelocity (also entered by
 * Mot_SteerToHeading): d1 toward d4 by d3, d2 toward d6 by d5, stored as the velocity (and its
 * copy +$38 / +$3C), then the impulses (Op2_StepVelocity_Impulse, which it falls into).
 * In:  d1 / d2 = the velocity, d3 / d5 = the accelerations, d4 / d6 = the target speeds; mot
 * Out: mot->vel_x/y = last_vel_x/y = d1 / d2; then as Op2_StepVelocity_Impulse
 * Evidence: cmp.l d4,d1 / blt / sub.l d3,d1 / cmp.l d4,d1 / bgt ... move.l d4,d1, the same on y;
 *           movem.l d1-d2,$18(a3) / movem.l d1-d2,$38(a3); bra.w from $156A (three sites)
 */
void Op2_StepVelocity_Steer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5,
                            uint32_t d6, uint32_t *a0, const u16 **script, Vm2Motion *mot)
{
    *d1 = steer(*d1, d3, d4);
    *d2 = steer(*d2, d5, d6);
    mot->vel_x = (int32_t)*d1;
    mot->vel_y = (int32_t)*d2;
    mot->last_vel_x = (int32_t)*d1;
    mot->last_vel_y = (int32_t)*d2;
    Op2_StepVelocity_Impulse(d0, d1, d2, a0, script, mot);
}

/* Op2_StepVelocity_Impulse - the impulse tail of Op2_StepVelocity (also entered by
 * Mot_SteerToHeading while stunned): the impulses decay by $1400 toward 0 and are added to the move.
 * In:  d1 / d2 = the move so far; mot->impulse_x / impulse_y
 * Out: d1 / d2 += the decayed impulses; d0 = the y impulse (or the x one when y is 0); NEXT
 *      (VM2_Next when the y impulse is 0)
 * Evidence: move.l $2(a3),d0 / beq / bpl / addi.l #$1400 / bmi ... subi.l #$1400 / bpl / moveq #0 /
 *           move.l d0,$2(a3) / add.l d0,d1, the same on $6(a3) and d2 (beq.w VM2_Next); bra.w from $156A
 */
void Op2_StepVelocity_Impulse(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *a0, const u16 **script,
                              Vm2Motion *mot)
{
    *d0 = (uint32_t)mot->impulse_x;
    if (*d0 != 0) {
        *d0 = decay(*d0);
        mot->impulse_x = (int32_t)*d0;
        *d1 += *d0;
    }
    *d0 = (uint32_t)mot->impulse_y;
    if (*d0 == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    *d0 = decay(*d0);
    mot->impulse_y = (int32_t)*d0;
    *d2 += *d0;
    op_next(a0, script);
}

/* Op2_ClampMoveToArea - a2 op: trims this frame's move so the object stays inside the movement
 * area: y first, then x (Op2_ClampMoveToAreaX, which it falls into).
 * In:  d1 / d2 = the move (Op2_StepVelocity); self position; g_MoveAreaTop / g_MoveAreaH
 * Out: d2 trimmed so y + d2 lies in [top, top + h); d0 = the overshoot; then Op2_ClampMoveToAreaX
 * Evidence: move.l $42(a6),d0 / add.l d2,d0 / sub.l (g_MoveAreaTop).w,d0 / bmi / sub.l
 *           (g_MoveAreaH).w,d0 / bmi.w Op2_ClampMoveToAreaX / sub.l d0,d2; 71 corpus runs;
 *           $4CF84 `4C08 4C52`
 */
void Op2_ClampMoveToArea(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    *d0 = pos_y(self) + *d2 - g_MoveAreaTop;
    if ((int32_t)*d0 >= 0) {
        *d0 -= area_h();
        if ((int32_t)*d0 < 0) {
            Op2_ClampMoveToAreaX(d0, d1, a0, script, self);
            return;
        }
    }
    *d2 -= *d0;
    Op2_ClampMoveToAreaX(d0, d1, a0, script, self);
}

/* Op2_ClampMoveToAreaX - a2 op: trims only the x move to keep the object between g_MoveAreaLeft
 * and g_MoveAreaLeft + g_MoveAreaW.
 * In:  d1 = the x move; self position
 * Out: d1 trimmed; d0 = the overshoot; NEXT (VM2_Next when inside)
 * Evidence: move.l $3E(a6),d0 / add.l d1,d0 / sub.l (g_MoveAreaLeft).w,d0 / bmi / sub.l
 *           (g_MoveAreaW).w,d0 / bmi.w VM2_Next / sub.l d0,d1; also dispatched alone (89 corpus
 *           runs); $3C734 `4C1E 4CDA`
 */
void Op2_ClampMoveToAreaX(uint32_t *d0, uint32_t *d1, uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    /* native: the area widened to the wide view (the gameplay mode, R20): its left edge out by the
       px the view shows left of the 4:3 view, its width by both sides'; 0 otherwise */
    uint32_t wide_l = (uint32_t)WS_AREA_L() << 16, wide_w = (uint32_t)(uint16_t)(WS_AREA_L() + WS_AREA_R()) << 16;
    *d0 = pos_x(self) + *d1 - (g_MoveAreaLeft - wide_l);
    if ((int32_t)*d0 >= 0) {
        *d0 -= area_w() + wide_w;
        if ((int32_t)*d0 < 0) {
            dk::jump(at_VM2_Next);
            return;
        }
    }
    *d1 -= *d0;
    op_next(a0, script);
}

/* Op2_ClampMoveToAreaY - a2 op: trims only the y move to keep the object between g_MoveAreaTop and
 * g_MoveAreaTop + g_MoveAreaH.
 * In:  d2 = the y move; self position
 * Out: d2 trimmed; d0 = the overshoot; NEXT (VM2_Next when inside)
 * Evidence: move.l $42(a6),d0 / add.l d2,d0 / sub.l (g_MoveAreaTop).w,d0 / bmi / sub.l
 *           (g_MoveAreaH).w,d0 / bmi.w VM2_Next / sub.l d0,d2; 8 corpus runs; $424AA `4C38 4C52`
 */
void Op2_ClampMoveToAreaY(uint32_t *d0, uint32_t *d2, uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    *d0 = pos_y(self) + *d2 - g_MoveAreaTop;
    if ((int32_t)*d0 >= 0) {
        *d0 -= area_h();
        if ((int32_t)*d0 < 0) {
            dk::jump(at_VM2_Next);
            return;
        }
    }
    *d2 -= *d0;
    op_next(a0, script);
}

/* Op2_ApplyMove - a2 op: adds this frame's move to the object's position, no terrain test.
 * In:  d1 / d2 = the move (16.16)
 * Out: self x / y moved; NEXT
 * Evidence: add.l d1,$3E(a6) / add.l d2,$42(a6) / NEXT; 143 corpus runs; $11B84 `4B74 4C52`
 */
void Op2_ApplyMove(uint32_t d1, uint32_t d2, uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    set_pos_x(self, pos_x(self) + d1);
    set_pos_y(self, pos_y(self) + d2);
    op_next(a0, script);
}
