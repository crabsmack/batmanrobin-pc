#include "game.h"

/* The random screen targets and the speed-setup ops: accelerations from a distance and a time
 * (Mot_AccelForDistance), cruise speeds and velocities as word * the speed factor a4 +$2E
 * ($001736-$0017E1). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the screen centre the random targets are picked around (screen pixels) */
static constexpr uint16_t CENTRE_X = 0xA0, CENTRE_Y = 0x70;

/* Op2_TargetRandomScreen - a2 op: a random target point near the screen centre: x = 160 +
 * random within +-75, y = 112 + random within +-55 (screen pixels, compared by the screen seek
 * ops with x - g_CameraX / y - g_CameraY).
 * In:  -
 * Out: mot->point_x / point_y; NEXT
 * Evidence: move.w #$96,d0 / jsr Rand_Signed / addi.w #$A0,d0 / move.w d0,$14(a3) / move.w #$6E,d0 /
 *           jsr Rand_Signed / addi.w #$70,d0 / move.w d0,$16(a3) / NEXT; TAS frames 116871-117395
 */
void Op2_TargetRandomScreen(const u16 **script, Vm2Motion *mot)
{
    uint32_t handler, r = 0x96;
    Rand_Signed(&r);
    mot->point_x = (int16_t)(uint16_t)(r + CENTRE_X);
    r = 0x6E;
    Rand_Signed(&r);
    mot->point_y = (int16_t)(uint16_t)(r + CENTRE_Y);
    next(&handler, script);
}

/* Op2_TargetRandomScreenArea - a2 op: a random target point around the screen centre (160, 112)
 * within the given x and y ranges.
 * In:  script = word x range, word y range (random within +-range/2 each)
 * Out: r = the last Rand_Signed result + $70 (its low word); mot->point_x / point_y; NEXT
 * Evidence: as Op2_TargetRandomScreen with move.w (a2)+,d0 for the ranges; 3 corpus runs incl.
 *           the TAS
 */
void Op2_TargetRandomScreenArea(uint32_t *r, const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    *r = (*r & 0xFFFF0000u) | *(*script)++;
    Rand_Signed(r);
    *r = (*r & 0xFFFF0000u) | (uint16_t)(*r + CENTRE_X);
    mot->point_x = (int16_t)(uint16_t)*r;
    *r = (*r & 0xFFFF0000u) | *(*script)++;
    Rand_Signed(r);
    *r = (*r & 0xFFFF0000u) | (uint16_t)(*r + CENTRE_Y);
    mot->point_y = (int16_t)(uint16_t)*r;
    next(&handler, script);
}

/* Op2_SetAccelForDist - a2 op: both accelerations = what Mot_AccelForDistance computes from a
 * distance and a time (the steering tail $4B8A moves the velocities toward the target speeds by
 * these steps).
 * In:  script = word distance, word time
 * Out: the registers as Mot_AccelForDistance leaves them; mot->accel_x / accel_y; NEXT
 * Evidence: bsr.w Mot_AccelForDistance / move.l d1,$20(a3) / move.l d1,$28(a3) / NEXT; 94 corpus
 *           runs incl. the TAS
 */
void Op2_SetAccelForDist(uint16_t *speed, uint32_t *accel, uint32_t *series, uint16_t *time,
                         const u16 **script, Vm2Motion *mot, const Vm2Block *vm)
{
    uint32_t handler;
    Mot_AccelForDistance(speed, accel, series, time, script, vm);
    mot->accel_x = (int32_t)*accel;
    mot->accel_y = (int32_t)*accel;
    next(&handler, script);
}

/* Op2_SetCruise - a2 op: both cruise speeds = word * the speed factor (signed).
 * In:  script = word speed; vm->speed
 * Out: cruise = the product (muls.w); mot->cruise_x / cruise_y; handler = the next op's; NEXT
 * Evidence: move.w (a2)+,d0 / muls.w $2E(a4),d0 / move.l d0,$30(a3) / move.l d0,$34(a3) / NEXT; 85
 *           corpus runs incl. the TAS
 */
void Op2_SetCruise(uint32_t *cruise, uint32_t *handler, const u16 **script, Vm2Motion *mot,
                   const Vm2Block *vm)
{
    *cruise = (uint32_t)((int32_t)(int16_t)*(*script)++ * (int16_t)vm->speed);
    mot->cruise_x = (int32_t)*cruise;
    mot->cruise_y = (int32_t)*cruise;
    next(handler, script);
}

/* Op2_ClearImpulseX - a2 op: cancels the decaying x impulse the steering tail adds to the x
 * velocity.
 * In:  -
 * Out: mot->impulse_x = 0; NEXT
 * Evidence: clr.l $2(a3) / NEXT; $4BBE adds $2(a3) to the x velocity and decays it by $1400 per
 *           run; seen in 1 fuzz run only (not the TAS)
 */
void Op2_ClearImpulseX(const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    mot->impulse_x = 0;
    next(&handler, script);
}

/* Op2_SetAccelXForDist - a2 op: the x acceleration from a distance and a time
 * (Mot_AccelForDistance).
 * In:  script = word distance, word time
 * Out: the registers as Mot_AccelForDistance leaves them; mot->accel_x; NEXT
 * Evidence: bsr.w Mot_AccelForDistance / move.l d1,$20(a3) / NEXT; 152 corpus runs incl. the TAS
 */
void Op2_SetAccelXForDist(uint16_t *speed, uint32_t *accel, uint32_t *series, uint16_t *time,
                          const u16 **script, Vm2Motion *mot, const Vm2Block *vm)
{
    uint32_t handler;
    Mot_AccelForDistance(speed, accel, series, time, script, vm);
    mot->accel_x = (int32_t)*accel;
    next(&handler, script);
}

/* Op2_SetCruiseX - a2 op: the x cruise speed = word * the speed factor (signed).
 * In:  script = word speed; vm->speed
 * Out: cruise = the product (muls.w); mot->cruise_x; handler = the next op's; NEXT
 * Evidence: move.w (a2)+,d0 / muls.w $2E(a4),d0 / move.l d0,$30(a3) / NEXT; 152 corpus runs incl.
 *           the TAS
 */
void Op2_SetCruiseX(uint32_t *cruise, uint32_t *handler, const u16 **script, Vm2Motion *mot,
                    const Vm2Block *vm)
{
    *cruise = (uint32_t)((int32_t)(int16_t)*(*script)++ * (int16_t)vm->speed);
    mot->cruise_x = (int32_t)*cruise;
    next(handler, script);
}

/* Op2_SetVelXForward - a2 op: the x velocity = word * the speed factor (unsigned) in the facing
 * direction (negated while the sprite is H-flipped).
 * In:  script = word speed; vm->speed; spr->attr
 * Out: vel = the product (mulu.w); mot->vel_x; NEXT (through VM2_Next when not flipped)
 * Evidence: move.w (a2)+,d0 / mulu.w $2E(a4),d0 / move.l d0,$18(a3) / btst #3,$38(a5) /
 *           beq.w VM2_Next / neg.l $18(a3) / NEXT; 47 corpus runs incl. the TAS
 */
void Op2_SetVelXForward(uint32_t *vel, uint32_t *handler, const u16 **script, Vm2Motion *mot,
                        const Vm2Block *vm, const Vm2Sprite *spr)
{
    *vel = (uint32_t)*(*script)++ * vm->speed;
    mot->vel_x = (int32_t)*vel;
    if (!(spr->attr & 0x0800)) {
        dk::jump(at_VM2_Next);
        return;
    }
    mot->vel_x = (int32_t)(0u - (uint32_t)mot->vel_x);
    next(handler, script);
}
