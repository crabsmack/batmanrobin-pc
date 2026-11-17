#include "game.h"

/* The y-axis motion ops: braking, the jump and fall ops (gravity from Mot_JumpGravity, launch
 * speed from Mot_JumpLaunchSpeed, the sprite's velocity-to-frame mapping a5 +$32/$36), the
 * rising tests and the landing bounces ($001C3C-$001D7D). */

/* divs.w: the 32-bit dividend by a signed word; the remainder in the high word, the quotient in
   the low. A quotient that doesn't fit a word sets V and leaves the dividend unchanged. */
static uint32_t divs_w(uint32_t dividend, int16_t divisor, bool *overflow)
{
    int64_t q = (int64_t)(int32_t)dividend / divisor;
    *overflow = q < -32768 || q > 32767;
    if (*overflow)
        return dividend;
    int32_t r = (int32_t)dividend % divisor;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

static uint32_t neg(uint32_t v) { return 0u - v; }

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the frame mapping of a jump: bias = -launch speed, divisor = that / 50 (the frame ops pick a
   case 0-100 from launch to the matching fall speed) */
static void map_frames_to_launch(uint32_t *launch, Vm2Sprite *spr)
{
    bool overflow;
    *launch = neg(*launch);
    spr->frame_bias = (int32_t)*launch;
    *launch = divs_w(*launch, 0x32, &overflow);
    spr->frame_div = (uint16_t)*launch;
}

/* Op2_JumpAnim's tail ($1C90), with the gravity in d1: the y target speed = the cruise speed
   (the fall limit), both negated for a negative gravity; the launch speed added to the y velocity
   and the frame mapping set from it; NEXT */
static void jump_anim_tail(uint32_t *d0, uint32_t *d1, const u16 **script, Vm2Motion *mot,
                           const Vm2Block *vm, Vm2Sprite *spr)
{
    uint32_t handler;
    mot->target_y = mot->cruise_y;
    mot->accel_y = (int32_t)*d1;
    if ((int32_t)*d1 < 0) {
        mot->accel_y = (int32_t)neg((uint32_t)mot->accel_y);
        mot->target_y = (int32_t)neg((uint32_t)mot->target_y);
    }
    Mot_JumpLaunchSpeed(d0, d1, *script, vm);
    mot->vel_y = (int32_t)((uint32_t)mot->vel_y + *d1);
    map_frames_to_launch(d1, spr);
    next(&handler, script);
}

/* Op2_ClearTargetSpeedY - a2 op: the y target speed = 0, so the steering tail brakes the y
 * velocity by the y acceleration.
 * In:  -
 * Out: mot->target_y = 0; handler = the next op's; NEXT
 * Evidence: clr.l $2C(a3) / NEXT; 100 corpus runs incl. the TAS
 */
void Op2_ClearTargetSpeedY(uint32_t *handler, const u16 **script, Vm2Motion *mot)
{
    mot->target_y = 0;
    next(handler, script);
}

/* Op2_StopY - a2 op: stops vertical motion at once.
 * In:  -
 * Out: mot->target_y = mot->vel_y = 0; NEXT
 * Evidence: clr.l $2C(a3) / clr.l $1C(a3) / NEXT; 131 corpus runs incl. the TAS
 */
void Op2_StopY(const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    mot->target_y = 0;
    mot->vel_y = 0;
    next(&handler, script);
}

/* Op2_SetJumpGravity - a2 op: only the y acceleration = the gravity of a jump of the given
 * height and time, the velocity left alone.
 * In:  script = word height, word time
 * Out: the registers as Mot_JumpGravity leaves them; mot->accel_y; NEXT
 * Evidence: bsr.w Mot_JumpGravity / move.l d1,$28(a3) / NEXT; 89 corpus runs incl. the TAS
 */
void Op2_SetJumpGravity(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script,
                        Vm2Motion *mot, const Vm2Block *vm)
{
    uint32_t handler;
    Mot_JumpGravity(d0, d1, d2, d3, script, vm);
    mot->accel_y = (int32_t)*d1;
    next(&handler, script);
}

/* Op2_BranchIfNotRising - a2 op: branches when the y velocity is >= 0 (falling or still).
 * In:  script = word branch offset; mot->vel_y
 * Out: the branch (script + offset, NEXT), else Op2_SkipWord
 * Evidence: tst.l $1C(a3) / bmi.w Op2_SkipWord / adda.w (a2),a2 / NEXT; 94 corpus runs incl. the TAS
 */
void Op2_BranchIfNotRising(uint32_t *handler, const u16 **script, const Vm2Motion *mot)
{
    if (mot->vel_y < 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}

/* Op2_BranchIfRising - a2 op: branches when the y velocity is negative (moving up).
 * In:  script = word branch offset; mot->vel_y
 * Out: the branch (script + offset, NEXT), else Op2_SkipWord
 * Evidence: tst.l $1C(a3) / bpl.w Op2_SkipWord / adda.w (a2),a2 / NEXT; 33 corpus runs incl. the TAS
 */
void Op2_BranchIfRising(uint32_t *handler, const u16 **script, const Vm2Motion *mot)
{
    if (mot->vel_y >= 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}

/* Op2_JumpAnimRandom - a2 op: Op2_JumpAnim with a random height: random below the first word,
 * plus the second word.
 * In:  script = word height range, word base height, word time
 * Out: d0 = the height, then as Mot_JumpGravityD1 and the Op2_JumpAnim tail leave them; NEXT
 * Evidence: move.w (a2)+,d0 / bsr.w Rand_Below / add.w (a2)+,d0 / andi.w #$FFFF,d0 / move.l d0,d1 /
 *           bsr.w Mot_JumpGravityD1 / bra.s $1C90; 3 fuzz corpus runs only (not the TAS)
 */
void Op2_JumpAnimRandom(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script,
                        Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr)
{
    *d0 = (*d0 & 0xFFFF0000u) | *(*script)++;
    Rand_Below(d0);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(*d0 + *(*script)++);
    *d1 = *d0;
    Mot_JumpGravityD1(d0, d1, d2, d3, script, vm);
    jump_anim_tail(d0, d1, script, mot, vm, spr);
}

/* Op2_JumpAnim - a2 op: starts a jump that adds to the current y velocity: the gravity of the
 * height and time, the y target speed = the cruise speed (the fall limit; both negated for a
 * negative height), the launch speed added to the y velocity, and the sprite's
 * velocity-to-frame mapping set for it.
 * In:  script = word height, word time
 * Out: the registers as the helpers leave them (d1 = launch / 50); mot->accel_y / target_y /
 *      vel_y; spr->frame_bias / frame_div; NEXT
 * Evidence: bsr.w Mot_JumpGravity / move.l $34,$2C / move.l d1,$28 / bpl.s / neg.l $28 / neg.l $2C /
 *           bsr.w Mot_JumpLaunchSpeed / add.l d1,$1C / neg.l d1 / move.l d1,$32(a5) / divs.w #$32,d1 /
 *           move.w d1,$36(a5); 105 corpus runs incl. the TAS
 */
void Op2_JumpAnim(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script,
                  Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr)
{
    Mot_JumpGravity(d0, d1, d2, d3, script, vm);
    jump_anim_tail(d0, d1, script, mot, vm, spr);
}

/* Op2_Jump - a2 op: starts a jump that peaks height px up after time units: the y gravity, the
 * y velocity = the launch speed (set, not added), the y target speed = the y cruise speed; the
 * sprite mapping left alone.
 * In:  script = word height, word time
 * Out: the registers as the helpers leave them; mot->accel_y / vel_y / target_y; NEXT
 * Evidence: bsr.w Mot_JumpGravity / move.l d1,$28(a3) / bsr.w Mot_JumpLaunchSpeed /
 *           move.l d1,$1C(a3) / move.l $34(a3),$2C(a3) / NEXT; 66 corpus runs incl. the TAS
 */
void Op2_Jump(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script,
              Vm2Motion *mot, const Vm2Block *vm)
{
    uint32_t handler;
    Mot_JumpGravity(d0, d1, d2, d3, script, vm);
    mot->accel_y = (int32_t)*d1;
    Mot_JumpLaunchSpeed(d0, d1, *script, vm);
    mot->vel_y = (int32_t)*d1;
    mot->target_y = mot->cruise_y;
    next(&handler, script);
}

/* Op2_FallAnim - a2 op: starts a fall from rest with the gravity of a jump (height, time): the
 * object moved down 1 px first, then as Op2_FallAnimInPlace.
 * In:  script = word height, word time; self->y
 * Out: self->y + 1; then as Op2_FallAnimInPlace
 * Evidence: addq.w #1,$42(a6), then Op2_FallAnimInPlace's code; 9 corpus runs incl. the TAS
 */
void Op2_FallAnim(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script,
                  Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr, Vm2Obj *self)
{
    self->y = (int16_t)(self->y + 1);
    Op2_FallAnimInPlace(d0, d1, d2, d3, script, mot, vm, spr, self);
}

/* Op2_FallAnimInPlace - a2 op: a fall from rest with the gravity of a jump (height, time): the
 * y gravity set, the y velocity 0, the y target speed = the y cruise speed, the sprite's frame
 * mapping as for the matching jump (Op2_FallAnim without its 1-pixel nudge).
 * In:  script = word height, word time
 * Out: the registers as the helpers leave them (d1 = launch / 50); mot->accel_y, vel_y = 0,
 *      target_y; spr->frame_bias / frame_div; NEXT
 * Evidence: bsr.w Mot_JumpGravity / move.l d1,$28(a3) / bsr.w Mot_JumpLaunchSpeed / clr.l $1C(a3) /
 *           neg.l d1 / move.l d1,$32(a5) / divs.w #$32,d1 / move.w d1,$36(a5) / move.l $34,$2C;
 *           script handler word $1CDC (the street clowns, gunmen, bomb rabbits fall in with it)
 */
void Op2_FallAnimInPlace(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script,
                         Vm2Motion *mot, const Vm2Block *vm, Vm2Sprite *spr, Vm2Obj *self)
{
    uint32_t handler;
    Mot_JumpGravity(d0, d1, d2, d3, script, vm);
    mot->accel_y = (int32_t)*d1;
    Mot_JumpLaunchSpeed(d0, d1, *script, vm);
    mot->vel_y = 0;
    map_frames_to_launch(d1, spr);
    mot->target_y = mot->cruise_y;
    next(&handler, script);
}

/* the frame mapping of a straight move: divisor = the cruise speed / 100 (at least 1), bias 0.
   A quotient that overflows (|cruise| >= 50 px per run) leaves the dividend, which is nonzero. */
static void map_frames_to_cruise(uint32_t *d0, int32_t cruise, Vm2Sprite *spr)
{
    bool overflow;
    *d0 = divs_w((uint32_t)cruise, 0x64, &overflow);
    if (!overflow && (uint16_t)*d0 == 0)
        *d0 = 1;
    spr->frame_div = (uint16_t)*d0;
    spr->frame_bias = 0;
}

/* Op2_AnimByCruiseY - a2 op: the sprite's velocity-to-frame mapping for a straight move from the
 * y cruise speed.
 * In:  mot->cruise_y
 * Out: d0 = divs.w(cruise_y, 100) (1 when the quotient is 0); spr->frame_div, frame_bias = 0; NEXT
 * Evidence: move.l $34(a3),d0 / divs.w #$64,d0 / bne.s / moveq #1,d0 / move.w d0,$36(a5) /
 *           clr.l $32(a5); read by the frame ops $3EE2/$3F1C; 43 corpus runs incl. the TAS
 */
void Op2_AnimByCruiseY(uint32_t *d0, const u16 **script, const Vm2Motion *mot, Vm2Sprite *spr)
{
    uint32_t handler;
    map_frames_to_cruise(d0, mot->cruise_y, spr);
    next(&handler, script);
}

/* Op2_AnimByCruiseX - a2 op: the sprite's velocity-to-frame mapping from the x cruise speed.
 * In:  mot->cruise_x
 * Out: d0 = divs.w(cruise_x, 100) (1 when the quotient is 0); spr->frame_div, frame_bias = 0; NEXT
 * Evidence: move.l $30(a3),d0 / divs.w #$64,d0 / bne.s / moveq #1,d0 / move.w d0,$36(a5) /
 *           clr.l $32(a5); read by $3F1C (x velocity form); 25 corpus runs incl. the TAS
 */
void Op2_AnimByCruiseX(uint32_t *d0, const u16 **script, const Vm2Motion *mot, Vm2Sprite *spr)
{
    uint32_t handler;
    map_frames_to_cruise(d0, mot->cruise_x, spr);
    next(&handler, script);
}

/* Op2_BounceY - a2 op: bounces after a landing: when half the last y velocity exceeds the y
 * gravity, the y velocity becomes minus that (up at half speed); otherwise vertical motion stops.
 * In:  mot->last_vel_y, accel_y
 * Out: d0 = last_vel_y / 2 (negated when it bounces); mot->vel_y (and target_y = 0 when
 *      stopped); NEXT
 * Evidence: move.l $3C(a3),d0 / asr.l #1,d0 / cmp.l $28(a3),d0 / bgt.s / clr.l $1C / clr.l $2C /
 *           neg.l d0 / move.l d0,$1C(a3); 13 corpus runs incl. the TAS
 */
void Op2_BounceY(uint32_t *d0, uint32_t *handler, const u16 **script, Vm2Motion *mot)
{
    *d0 = (uint32_t)(mot->last_vel_y >> 1);
    if ((int32_t)*d0 > mot->accel_y) {
        *d0 = neg(*d0);
        mot->vel_y = (int32_t)*d0;
        next(handler, script);
        return;
    }
    mot->vel_y = 0;
    mot->target_y = 0;
    next(handler, script);
}

/* Op2_BounceYOrBranch - a2 op: bounces like Op2_BounceY while half the last y velocity exceeds
 * $6000 (0.375 px per run) and continues; once the bounce has died out it stops vertical motion
 * and branches.
 * In:  script = word branch offset; mot->last_vel_y
 * Out: d0 = last_vel_y / 2 (negated when it bounces); mot->vel_y (target_y = 0 when stopped); the
 *      branch (stopped), else NEXT past the offset
 * Evidence: move.l $3C(a3),d0 / asr.l #1,d0 / cmpi.l #$6000,d0 / bgt.s; clr.l $1C / clr.l $2C /
 *           adda.w (a2),a2 / NEXT; neg.l d0 / move.l d0,$1C / addq.l #2,a2 / NEXT; 77 corpus runs
 *           incl. the TAS
 */
void Op2_BounceYOrBranch(uint32_t *d0, uint32_t *handler, const u16 **script, Vm2Motion *mot)
{
    *d0 = (uint32_t)(mot->last_vel_y >> 1);
    if ((int32_t)*d0 > 0x6000) {
        *d0 = neg(*d0);
        mot->vel_y = (int32_t)*d0;
        (*script)++;
        next(handler, script);
        return;
    }
    mot->vel_y = 0;
    mot->target_y = 0;
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}
