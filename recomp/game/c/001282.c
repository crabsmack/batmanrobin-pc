#include "game.h"

/* The saved-position and heading-setup ops ($001282-$0012E3). Positions are 16.16 longs: a6
 * +$3E/$42 the object's, +$46/$4A the saved copy. */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the object's x / y as the 16.16 longs at +$3E / +$42 */
static uint32_t pos_x(const Vm2Obj *o) { return (uint32_t)(uint16_t)o->x << 16 | o->x_frac; }
static uint32_t pos_y(const Vm2Obj *o) { return (uint32_t)(uint16_t)o->y << 16 | o->y_frac; }
static void set_pos_x(Vm2Obj *o, uint32_t v) { o->x = (int16_t)(v >> 16); o->x_frac = (uint16_t)v; }
static void set_pos_y(Vm2Obj *o, uint32_t v) { o->y = (int16_t)(v >> 16); o->y_frac = (uint16_t)v; }

/* Op2_SavePos - a2 op: saves the object's position in the saved copy.
 * In:  self's position
 * Out: self->saved_x / saved_y; NEXT
 * Evidence: move.l $42(a6),$4A(a6), then Op2_SaveX's code; 3 TAS frames
 */
void Op2_SavePos(const u16 **script, Vm2Obj *self)
{
    self->saved_y = (int32_t)pos_y(self);
    Op2_SaveX(script, self);
}

/* Op2_SaveX - a2 op: saves only the object's x (Op2_SavePos past its y copy).
 * In:  self->x
 * Out: self->saved_x; NEXT
 * Evidence: move.l $3E(a6),$46(a6) / NEXT; a computed-jump target in the corpus
 */
void Op2_SaveX(const u16 **script, Vm2Obj *self)
{
    uint32_t handler;
    self->saved_x = (int32_t)pos_x(self);
    next(&handler, script);
}

/* Op2_SwapSavedPos - a2 op: exchanges the object's position with the saved one.
 * In:  self's position and saved position
 * Out: swapped; d0/d1 = the old saved x/y, d2/d3 = the old x/y (as the movem left them); NEXT
 * Evidence: movem.l $3E(a6),d0-d3 / exg.l d0,d2 / exg.l d1,d3 / movem.l d0-d3,$3E(a6); executed in the TAS
 */
void Op2_SwapSavedPos(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *handler,
                      const u16 **script, Vm2Obj *self)
{
    *d0 = (uint32_t)self->saved_x;
    *d1 = (uint32_t)self->saved_y;
    *d2 = pos_x(self);
    *d3 = pos_y(self);
    set_pos_x(self, *d0);
    set_pos_y(self, *d1);
    self->saved_x = (int32_t)*d2;
    self->saved_y = (int32_t)*d3;
    next(handler, script);
}

/* Op2_StepSavedPos - a2 op: moves the saved position by the motion block's velocity.
 * In:  mot->vel_x / vel_y
 * Out: self->saved_x / saved_y + the velocity; d0 = vel_y; NEXT
 * Evidence: move.l $18(a3),d0 / add.l d0,$46(a6) / move.l $1C(a3),d0 / add.l d0,$4A(a6); executed in the TAS
 */
void Op2_StepSavedPos(uint32_t *d0, const u16 **script, const Vm2Motion *mot, Vm2Obj *self)
{
    uint32_t handler;
    self->saved_x = (int32_t)((uint32_t)self->saved_x + (uint32_t)mot->vel_x);
    *d0 = (uint32_t)mot->vel_y;
    self->saved_y = (int32_t)((uint32_t)self->saved_y + *d0);
    next(&handler, script);
}

/* Op2_SetTurnRate - a2 op: the heading turn rate (16.16 heading units per run) = operand x the
 * speed factor (signed).
 * In:  script = word rate; vm->speed
 * Out: rate = mot->turn_rate (the muls product); NEXT
 * Evidence: move.w (a2)+,d0 / muls.w $2E(a4),d0 / move.l d0,$10(a3); Mot_TurnTowardHeading steps by
 *           it; executed in the TAS
 */
void Op2_SetTurnRate(uint32_t *rate, const u16 **script, Vm2Motion *mot, const Vm2Block *vm)
{
    uint32_t handler;
    *rate = (uint32_t)((int32_t)(int16_t)(uint16_t)*(*script)++ * (int16_t)(uint16_t)vm->speed);
    mot->turn_rate = (int32_t)*rate;
    next(&handler, script);
}

/* Op2_SetHeading - a2 op: the target heading = operand & $3FF (0 = +x, $100 = +y, 1024 a turn).
 * In:  script = word heading
 * Out: heading = mot->heading_target; NEXT
 * Evidence: move.w (a2)+,d0 / andi.w #$3FF,d0 / move.w d0,$A(a3); executed in the TAS
 */
void Op2_SetHeading(uint16_t *heading, const u16 **script, Vm2Motion *mot)
{
    uint32_t handler;
    *heading = (uint16_t)(*(*script)++ & 0x3FF);
    mot->heading_target = *heading;
    next(&handler, script);
}
