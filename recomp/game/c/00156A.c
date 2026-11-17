#include "game.h"

/* The heading turn and steering tails, and the position / target-point ops ($00156A-$001707). */

/* Tbl_059B22: the sine table, a signed word per even heading (index & $3FE as a byte offset),
   1.0 = $80 after the asr #7; cosine a quarter turn on (+$100 bytes) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the camera's integer x / y (the high words of the 16.16 longs) */
static uint16_t camera_x(void) { return (uint16_t)(g_CameraX >> 16); }
static uint16_t camera_y(void) { return (uint16_t)(g_CameraY >> 16); }

/* the turn's distance test: (target - heading) mod a full turn ($400 << 16), against a half turn */
static int32_t turn_left(uint32_t target, uint32_t heading) { return (int32_t)((target - heading) & 0x3FFFFFFu); }

/* Mot_TurnTowardHeading - turns the current heading (16.16) toward the target heading by the
 * turn rate, the shorter way round, landing on the target instead of passing it.
 * In:  mot->heading_target, heading, turn_rate
 * Out: mot->heading; target = the target << 16, heading = the new heading, diff = the last
 *      distance tested (all as the asm leaves d0-d2)
 * Evidence: moveq #0,d0 / move.w $A(a3),d0 / swap / sub.l / andi.l #$3FFFFFF / cmpi.l #$2000000 /
 *           bgt / add.l or sub.l $10(a3),d1, the test again, move.l d0,d1 on passing it;
 *           callers Op2_TurnSteer, Op2_TurnHeading; 15.8k TAS frames
 */
void Mot_TurnTowardHeading(uint32_t *target, uint32_t *heading, uint32_t *diff, Vm2Motion *mot)
{
    *target = (uint32_t)(uint16_t)mot->heading_target << 16;
    *heading = (uint32_t)mot->heading;
    int32_t d = turn_left(*target, *heading);
    bool keep;
    if (d > 0x2000000) {                  /* more than half a turn ahead: turn the other way */
        *heading -= (uint32_t)mot->turn_rate;
        d = turn_left(*target, *heading);
        keep = d > 0x2000000;
    } else {
        *heading += (uint32_t)mot->turn_rate;
        d = turn_left(*target, *heading);
        keep = d < 0x2000000;
    }
    *diff = (uint32_t)d;
    if (!keep)
        *heading = *target;               /* the step reached or passed it */
    mot->heading = (int32_t)*heading;
}

/* asr.l #8 / muls.w by a table value / asr.l #7: a 16.16 speed turned by a sine or cosine */
static uint32_t turn_speed(uint32_t speed, int16_t sincos)
{
    int32_t v = (int32_t)(int16_t)(uint16_t)(uint32_t)((int32_t)speed >> 8) * sincos;
    return (uint32_t)(v >> 7);
}

/* Mot_SteerToHeading - the steering tail of Op2_SnapHeadingSteer / Op2_TurnSteer: the target
 * speeds (x by cos, y by sin of the current heading) become the steps toward which
 * Op2_StepVelocity_Steer moves the velocity; with both target speeds 0 it brakes, the
 * accelerations turned the same way and made positive. While the stun count runs it only counts
 * down and lets the impulses decay (Op2_StepVelocity_Impulse with no velocity).
 * In:  mot->stun, vel, accel, target speeds, heading
 * Out: d1-d6 = vel_x, vel_y, accel_x, target_x, accel_y, target_y (the turned ones replaced),
 *      a0 = the table entry, d0 = target_x | target_y; jumped to Op2_StepVelocity_Steer
 *      (stunned: d1 = d2 = 0, Op2_StepVelocity_Impulse)
 * Evidence: tst.b $1(a3) / subq.b / bra.w Op2_StepVelocity_Impulse; movem.l $18(a3),d1-d6 /
 *           andi.w #$3FE / lea Tbl_059B22 / or.l / asr.l #8 / muls.w $100(a0) / (a0) / asr.l #7 /
 *           neg.l; executed in the TAS
 */
void Mot_SteerToHeading(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *a0, uint32_t *a2, Vm2Motion *mot)
{
    if (mot->stun != 0) {
        mot->stun = (uint8_t)(mot->stun - 1);
        *d1 = 0;
        *d2 = 0;
        dk::jump(at_Op2_StepVelocity_Impulse);
        return;
    }
    *d1 = (uint32_t)mot->vel_x;
    *d2 = (uint32_t)mot->vel_y;
    *d3 = (uint32_t)mot->accel_x;
    *d4 = (uint32_t)mot->target_x;
    *d5 = (uint32_t)mot->accel_y;
    *d6 = (uint32_t)mot->target_y;
    uint16_t i = (uint16_t)(((uint32_t)mot->heading >> 16) & 0x3FE);
    *a0 = Tbl_059B22 + i;
    PTR(const s16) e;
    e.v = *a0;
    const s16 *entry = e;
    int16_t sin = entry[0], cos = entry[0x80];
    *d0 = *d4 | *d6;
    if (*d0 != 0) {
        *d4 = turn_speed(*d4, cos);
        *d6 = turn_speed(*d6, sin);
    } else {
        *d3 = turn_speed(*d3, cos);
        if ((int32_t)*d3 < 0)
            *d3 = (uint32_t)-(int32_t)*d3;
        *d5 = turn_speed(*d5, sin);
        if ((int32_t)*d5 < 0)
            *d5 = (uint32_t)-(int32_t)*d5;
    }
    dk::jump(at_Op2_StepVelocity_Steer);
}

/* Op2_SetY's code, with the handler for Op2_SetPos (which returns it in a0) */
static void set_y(uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    self->y = (int16_t)(uint16_t)*(*script)++;
    self->y_frac = 0;
    next(handler, script);
}

/* Op2_SetPos - a2 op: places the object at the world point (x, y), fractions 0.
 * In:  script = words x, y
 * Out: self's position; NEXT
 * Evidence: move.w (a2)+,$3E(a6) / clr.w $40(a6), then Op2_SetY's code; executed in the TAS
 */
void Op2_SetPos(uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    self->x = (int16_t)(uint16_t)*(*script)++;
    self->x_frac = 0;
    set_y(handler, script, self);
}

/* Op2_SetY - a2 op: the object's world y from a word, fraction 0 (Op2_SetPos past its x).
 * In:  script = word y
 * Out: self->y; NEXT
 * Evidence: move.w (a2)+,$42(a6) / clr.w $44(a6) / NEXT; a computed-jump target in the corpus
 */
void Op2_SetY(const u16 **script, Vm2Obj *self)
{
    uint32_t handler;
    set_y(&handler, script, self);
}

/* Op2_SetX - a2 op: the object's world x from a word, fraction 0.
 * In:  script = word x
 * Out: self->x; NEXT
 * Evidence: move.w (a2)+,$3E(a6) / clr.w $40(a6) / NEXT; executed in the TAS
 */
void Op2_SetX(const u16 **script, Vm2Obj *self)
{
    uint32_t handler;
    self->x = (int16_t)(uint16_t)*(*script)++;
    self->x_frac = 0;
    next(&handler, script);
}

/* Op2_MovePos - a2 op: moves the object by (dx, dy) whole pixels.
 * In:  script = words dx, dy
 * Out: self's position; delta = dy; NEXT
 * Evidence: move.w (a2)+,d0 / add.w d0,$3E(a6), then Op2_MoveY's code; executed in the TAS
 */
void Op2_MovePos(uint16_t *delta, const u16 **script, Vm2Obj *self)
{
    *delta = *(*script)++;
    self->x = (int16_t)(self->x + *delta);
    Op2_MoveY(delta, script, self);
}

/* Op2_MoveY - a2 op: moves the object vertically by dy whole pixels (Op2_MovePos past its x).
 * In:  script = word dy
 * Out: self->y; delta = dy; NEXT
 * Evidence: move.w (a2)+,d0 / add.w d0,$42(a6) / NEXT; a computed-jump target in the corpus
 */
void Op2_MoveY(uint16_t *delta, const u16 **script, Vm2Obj *self)
{
    uint32_t handler;
    *delta = *(*script)++;
    self->y = (int16_t)(self->y + *delta);
    next(&handler, script);
}

/* Op2_MoveX - a2 op: moves the object horizontally by dx whole pixels.
 * In:  script = word dx
 * Out: self->x; delta = dx; NEXT
 * Evidence: move.w (a2)+,d0 / add.w d0,$3E(a6) / NEXT; executed in the TAS
 */
void Op2_MoveX(uint16_t *delta, const u16 **script, Vm2Obj *self)
{
    uint32_t handler;
    *delta = *(*script)++;
    self->x = (int16_t)(self->x + *delta);
    next(&handler, script);
}

/* Op2_MoveXFacing - a2 op: moves the object by dx in its facing direction (dx negated when the
 * sprite is H-flipped: bit 3 of the attributes' high byte).
 * In:  script = word dx; spr->attr
 * Out: self->x; NEXT
 * Evidence: move.w (a2)+,d0 / btst #3,$38(a5) / beq / neg.w d0 / add.w d0,$3E(a6); executed in the TAS
 */
void Op2_MoveXFacing(uint32_t *handler, const u16 **script, const Vm2Sprite *spr, Vm2Obj *self)
{
    uint16_t dx = *(*script)++;
    if ((spr->attr >> 8) & 0x08)
        dx = (uint16_t)-dx;
    self->x = (int16_t)(self->x + dx);
    next(handler, script);
}

/* camera + operand as a 16.16 position (moveq #0 / move.w camera / add.w (a2)+ / swap) */
static uint32_t screen_pos(uint16_t camera, const u16 **script)
{
    return (uint32_t)(uint16_t)(camera + *(*script)++) << 16;
}

/* camera + the x operand as a 16.16 position, the operand placed for the wide view's edge (native:
 * the gameplay mode, R20; ws_place_x is the operand itself otherwise) */
static uint32_t screen_x_pos(const u16 **script, Vm2Obj *self)
{
    uint16_t sx = *(*script)++;
    uint16_t placed = ws_place_x(sx);
    /* the op's address: two words back */
    WS_NOTE_PLACE(WS_PLACE_ON_SCREEN, (int16_t)sx, (int16_t)(uint16_t)(placed - sx), dk::addr(self), dk::addr(*script) - 4);
    (void)self;
    return (uint32_t)(uint16_t)(camera_x() + placed) << 16;
}

static void place_on_screen(uint32_t *x, uint32_t *y, uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    *x = screen_x_pos(script, self);
    *y = screen_pos(camera_y(), script);
    self->x = (int16_t)(*x >> 16);
    self->x_frac = 0;
    self->y = (int16_t)(*y >> 16);
    self->y_frac = 0;
    next(handler, script);
}

/* Op2_PlaceOnScreen - a2 op: places the object at screen point (x, y) (world = camera +
 * operand, fractions 0) and stops it: velocity and target speeds cleared.
 * In:  script = words x, y
 * Out: self's position; x / y = the position longs; mot->vel and target speeds 0; NEXT
 * Evidence: clr.l $24/$18/$2C/$1C(a3), then Op2_PlaceOnScreenKeepVel's code; executed in the TAS
 */
void Op2_PlaceOnScreen(uint32_t *x, uint32_t *y, uint32_t *handler, const u16 **script, Vm2Motion *mot, Vm2Obj *self)
{
    mot->target_x = 0;
    mot->vel_x = 0;
    mot->target_y = 0;
    mot->vel_y = 0;
    place_on_screen(x, y, handler, script, self);
}

/* Op2_PlaceOnScreenKeepVel - a2 op: places the object at screen point (x, y) without stopping it
 * (Op2_PlaceOnScreen past its four clr.l).
 * In:  script = words x, y
 * Out: self's position; x / y = the position longs; NEXT
 * Evidence: moveq #0,d0 / move.w (g_CameraX).w,d0 / add.w (a2)+,d0 / swap ... / movem.l d0-d1,$3E(a6);
 *           a computed-jump target in the corpus
 */
void Op2_PlaceOnScreenKeepVel(uint32_t *x, uint32_t *y, uint32_t *handler, const u16 **script, Vm2Obj *self)
{
    place_on_screen(x, y, handler, script, self);
}

/* Op2_PlaceXOnScreen - a2 op: the object's x = camera x + operand (fraction 0); its x velocity
 * and x target speed cleared.
 * In:  script = word x
 * Out: self->x; x = the position long; NEXT
 * Evidence: clr.l $24(a3) / clr.l $18(a3) / moveq / move.w (g_CameraX).w,d0 / add.w (a2)+ / swap /
 *           move.l d0,$3E(a6); executed in the TAS
 */
void Op2_PlaceXOnScreen(uint32_t *x, const u16 **script, Vm2Motion *mot, Vm2Obj *self)
{
    uint32_t handler;
    mot->target_x = 0;
    mot->vel_x = 0;
    *x = screen_x_pos(script, self);
    self->x = (int16_t)(*x >> 16);
    self->x_frac = 0;
    next(&handler, script);
}

/* Op2_PlaceYOnScreen - a2 op: the object's y = camera y + operand (fraction 0); its y velocity
 * and y target speed cleared.
 * In:  script = word y
 * Out: self->y; y = the position long; NEXT
 * Evidence: clr.l $2C(a3) / clr.l $1C(a3), then Op2_PlaceYOnScreenKeepVel's code; executed in the TAS
 */
void Op2_PlaceYOnScreen(uint32_t *y, const u16 **script, Vm2Motion *mot, Vm2Obj *self)
{
    mot->target_y = 0;
    mot->vel_y = 0;
    Op2_PlaceYOnScreenKeepVel(y, script, self);
}

/* Op2_PlaceYOnScreenKeepVel - a2 op: the object's y = camera y + operand (fraction 0), its motion
 * untouched (Op2_PlaceYOnScreen past its two clr.l).
 * In:  script = word y
 * Out: self->y; y = the position long; NEXT
 * Evidence: moveq / move.w (g_CameraY).w,d0 / add.w (a2)+ / swap / move.l d0,$42(a6); a
 *           computed-jump target in the corpus
 */
void Op2_PlaceYOnScreenKeepVel(uint32_t *y, const u16 **script, Vm2Obj *self)
{
    uint32_t handler;
    *y = screen_pos(camera_y(), script);
    self->y = (int16_t)(*y >> 16);
    self->y_frac = 0;
    next(&handler, script);
}

/* Op2_SetTargetArgs - a2 op: the target point = (g_VM2Arg0, g_VM2Arg1), without re-aiming.
 * In:  g_VM2Arg0 / g_VM2Arg1 (the words at their addresses: the longs' high halves)
 * Out: mot->point_x / point_y; NEXT
 * Evidence: move.w (g_VM2Arg0).w,$14(a3) / move.w (g_VM2Arg1).w,$16(a3); executed in the TAS
 */
void Op2_SetTargetArgs(const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    mot->point_x = (int16_t)(uint16_t)(g_VM2Arg0 >> 16);
    mot->point_y = (int16_t)(uint16_t)(g_VM2Arg1 >> 16);
    next(&handler, script);
}

/* Op2_SetTarget - a2 op: the target point = the world point (x, y), without re-aiming.
 * In:  script = words x, y
 * Out: mot->point_x / point_y; NEXT
 * Evidence: move.w (a2)+,$14(a3), then Op2_SetTargetY's code; executed in the TAS
 */
void Op2_SetTarget(const u16 **script, Vm2Motion *mot)
{
    mot->point_x = (int16_t)(uint16_t)*(*script)++;
    Op2_SetTargetY(script, mot);
}

/* Op2_SetTargetY - a2 op: only the target point's y from a word (Op2_SetTarget past its x).
 * In:  script = word y
 * Out: mot->point_y; NEXT
 * Evidence: move.w (a2)+,$16(a3) / NEXT; a computed-jump target in the corpus
 */
void Op2_SetTargetY(const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    mot->point_y = (int16_t)(uint16_t)*(*script)++;
    next(&handler, script);
}

/* Op2_SetTargetX - a2 op: only the target point's x from a word.
 * In:  script = word x
 * Out: mot->point_x; NEXT
 * Evidence: move.w (a2)+,$14(a3) / NEXT; executed in the TAS
 */
void Op2_SetTargetX(const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    mot->point_x = (int16_t)(uint16_t)*(*script)++;
    next(&handler, script);
}
