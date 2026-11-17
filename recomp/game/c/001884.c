#include "game.h"

/* Op2_StopX, the x arc and the bursts, Op2_FaceTargetSpeedX and the entries of the screen-x seek
 * (Op2_SeekScreenX{,4}) ($001884-$00191F). */

static constexpr uint16_t ATTR_HFLIP = 0x0800;   /* bit 3 of the attributes' high byte */

static uint32_t neg(uint32_t v) { return 0u - v; }

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_StopX - a2 op: stops horizontal motion at once.
 * In:  -
 * Out: mot->target_x = mot->vel_x = 0; handler = the next op's; NEXT
 * Evidence: clr.l $24(a3) / clr.l $18(a3) / NEXT; 111 corpus runs incl. the TAS
 */
void Op2_StopX(uint32_t *handler, const u16 **script, Vm2Motion *mot)
{
    mot->target_x = 0;
    mot->vel_x = 0;
    next(handler, script);
}

/* Op2_ArcX - a2 op: the x-axis form of Op2_Jump: the x acceleration = the gravity of the
 * (distance, time) pair made positive, the x velocity = the matching launch speed, the x target
 * speed = the x cruise speed (its negation for a negative distance is overwritten, as in the
 * original).
 * In:  script = word distance, word time
 * Out: the registers as Mot_JumpGravity / Mot_JumpLaunchSpeed leave them; mot->accel_x / vel_x /
 *      target_x; NEXT
 * Evidence: bsr.w Mot_JumpGravity / move.l d1,$20(a3) / bpl.s / neg.l $20 / neg.l $24 /
 *           bsr.w Mot_JumpLaunchSpeed / move.l d1,$18 / move.l $30,$24 / NEXT; 52 corpus runs
 */
void Op2_ArcX(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script,
              Vm2Motion *mot, const Vm2Block *vm)
{
    uint32_t handler;
    Mot_JumpGravity(d0, d1, d2, d3, script, vm);
    mot->accel_x = (int32_t)*d1;
    if ((int32_t)*d1 < 0) {
        mot->accel_x = (int32_t)neg((uint32_t)mot->accel_x);
        mot->target_x = (int32_t)neg((uint32_t)mot->target_x);
    }
    Mot_JumpLaunchSpeed(d0, d1, *script, vm);
    mot->vel_x = (int32_t)*d1;
    mot->target_x = mot->cruise_x;
    next(&handler, script);
}

/* Op2_BurstX - a2 op: a short horizontal burst: the x velocity = 2 * word px per run in the
 * facing direction, the x target speed 0 and the x acceleration |word|, so it brakes to a stop.
 * In:  script = word speed (whole pixels); spr->attr (H flip)
 * Out: accel = |word| << 16; vel = 2 * (word << 16), negated when flipped; mot->target_x = 0,
 *      accel_x, vel_x; NEXT
 * Evidence: moveq #0,d0 / move.l d0,$24(a3) / move.w (a2)+,d0 / swap d0 / move.l d0,d1 / bpl.s /
 *           neg.l d0 / move.l d0,$20 / add.l d1,d1 / btst #3,$38(a5) / neg.l d1 / move.l d1,$18;
 *           20 corpus runs incl. the TAS
 */
void Op2_BurstX(uint32_t *accel, uint32_t *vel, const u16 **script, Vm2Motion *mot, const Vm2Sprite *spr)
{
    uint32_t handler;
    mot->target_x = 0;
    *accel = (uint32_t)*(*script)++ << 16;
    *vel = *accel;
    if ((int32_t)*accel < 0)
        *accel = neg(*accel);
    mot->accel_x = (int32_t)*accel;
    *vel += *vel;
    if (spr->attr & ATTR_HFLIP)
        *vel = neg(*vel);
    mot->vel_x = (int32_t)*vel;
    next(&handler, script);
}

/* Op2_BurstY - a2 op: a short vertical burst: the y velocity = 2 * word px per run (sign as
 * given, negative = up), the y target speed 0 and the y acceleration |word|.
 * In:  script = word speed (whole pixels)
 * Out: accel = |word| << 16; vel = 2 * (word << 16); mot->target_y = 0, accel_y, vel_y; NEXT
 * Evidence: Op2_BurstX's code on $2C/$28/$1C without the facing test; 5 corpus runs incl. the TAS
 */
void Op2_BurstY(uint32_t *accel, uint32_t *vel, const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    mot->target_y = 0;
    *accel = (uint32_t)*(*script)++ << 16;
    *vel = *accel;
    if ((int32_t)*accel < 0)
        *accel = neg(*accel);
    mot->accel_y = (int32_t)*accel;
    *vel += *vel;
    mot->vel_y = (int32_t)*vel;
    next(&handler, script);
}

/* Op2_FaceTargetSpeedX - a2 op: turns the sprite to face along the x target speed: the H flip
 * cleared when it is positive, set when negative, unchanged when 0. Only the target speed's
 * integer word is tested (tst.w of the 16.16 long), so a speed below 1 px per run counts as 0.
 * In:  mot->target_x (high word); spr->attr
 * Out: spr->attr's H flip; NEXT (through VM2_Next when 0)
 * Evidence: tst.w $24(a3) / beq.w VM2_Next / bmi.s / bclr #3,$38(a5) / NEXT / bset #3,$38(a5) /
 *           NEXT; 14 corpus runs incl. the TAS
 */
void Op2_FaceTargetSpeedX(uint32_t *handler, const u16 **script, const Vm2Motion *mot, Vm2Sprite *spr)
{
    int16_t speed = (int16_t)(uint16_t)((uint32_t)mot->target_x >> 16);
    if (speed == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    if (speed < 0)
        spr->attr = (uint16_t)(spr->attr | ATTR_HFLIP);
    else
        spr->attr = (uint16_t)(spr->attr & ~ATTR_HFLIP);
    next(handler, script);
}

/* Op2_SeekScreenX - a2 op: steers horizontally toward the target point's screen x, stopping
 * within the given tolerance.
 * In:  script = word tolerance (pixels)
 * Out: tolerance = the word; then Mot_SeekScreenX (jumped to)
 * Evidence: move.w (a2)+,d1 / bra.s Mot_SeekScreenX; 4 corpus runs incl. the TAS
 */
void Op2_SeekScreenX(uint16_t *d0, uint16_t *tolerance, uint32_t *handler, const u16 **script,
                     const Vm2Motion *mot, const Vm2Obj *self)
{
    *tolerance = *(*script)++;
    dk::jump(at_Mot_SeekScreenX);
}

/* Op2_SeekScreenX4 - a2 op: Mot_SeekScreenX with a 4-pixel tolerance.
 * In:  -
 * Out: tolerance = 4; then Mot_SeekScreenX (jumped to)
 * Evidence: moveq #4,d1 / bra.s Mot_SeekScreenX; 8 corpus runs incl. the TAS
 */
void Op2_SeekScreenX4(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script,
                      const Vm2Motion *mot, const Vm2Obj *self)
{
    *tolerance = 4;
    dk::jump(at_Mot_SeekScreenX);
}
