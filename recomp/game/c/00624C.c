#include "game.h"

/* $00624C-$0063AB: the a4 object's drawn point and motion. The drawn point is (x, y) + radius *
 * (cos, sin)(heading) through the sine table Tbl_059B22; the motion ops step velocity +=
 * acceleration, value += velocity (16.16 longs) and leave the drawn point's pixel words in the low
 * words of d4 / d5 for the terrain, touch and draw ops after them. */

/* Tbl_059B22: the sine table, a signed word per even heading (heading & $3FE as a byte offset);
   the cosine a quarter turn on (+$100 bytes) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;
static const s16 *sine_entry(uint16_t byte_index)
{
    PTR(const s16) e;
    e.v = Tbl_059B22 + byte_index;
    return e;
}

/* muls.w: the low word of a register (signed) times a table value */
static uint32_t muls(uint32_t d, int16_t m) { return (uint32_t)((int32_t)(int16_t)(uint16_t)d * m); }
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static uint32_t swap(uint32_t v) { return v << 16 | v >> 16; }

static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the drawn point from d4 / d5 = x / y (the tail of VM4_CalcDrawPos and Op4_Move): with a
 * radius, d0 / d1 = radius * cos / sin added to them; a0 = the table entry */
static void add_polar(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *a0, const Vm4Obj *self)
{
    set_low_word(d0, (uint16_t)self->radius);
    if ((uint16_t)*d0 == 0)
        return;
    uint16_t index = (uint16_t)(self->heading & 0x3FE);
    const s16 *e = sine_entry(index);
    *a0 = dk::addr(e);
    set_low_word(d1, (uint16_t)*d0);
    *d0 = muls(*d0, e[0x80]);   /* the cosine */
    *d1 = muls(*d1, e[0]);
    *d4 += *d0;
    *d5 += *d1;
}

/* VM4_CalcDrawPos - computes an a4 object's drawn point: d4 / d5 = (x, y) + radius * (cos, sin)
 * (heading), 16.16.
 * In:  a6 = the object (radius, heading, x, y)
 * Out: d4 / d5 = the drawn x / y; d0's low word = the radius, with one d0 / d1 = the products
 * Evidence: movem.l $2E(a6),d4-d5 / move.w $26(a6),d0 / beq rts / lea Tbl_059B22,a0 / andi.w #$3FE /
 *           muls.w $100(a0),d0 / muls.w (a0),d1 / add.l d0,d4 / add.l d1,d5; callers
 *           Op4_SpawnAtDrawPos, Op4_CalcDrawPos
 */
void VM4_CalcDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, const Vm4Obj *self)
{
    *d4 = vm4_fix(self->x, self->x_frac);
    *d5 = vm4_fix(self->y, self->y_frac);
    uint32_t table_entry;
    add_polar(d0, d1, d4, d5, &table_entry, self);
}

/* Op4_BounceY - a4 op: reverses and halves the object's y velocity (a bounce).
 * In:  script (no operands); self = a6
 * Out: d0 = the new y velocity; self->vel_y; NEXT
 * Evidence: move.l $42(a6),d0 / asr.l #1,d0 / neg.l d0 / move.l d0,$42(a6); script use $1F216
 *           (`6276 549A`); 3 corpus runs
 */
void Op4_BounceY(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = 0u - (uint32_t)((int32_t)self->vel_y >> 1);
    self->vel_y = (int32_t)*d0;
    uint32_t handler;
    next(&handler, script);
}

/* Op4_Move - a4 op: steps the object's full motion (each velocity += its acceleration; radius,
 * heading, x, y += velocity) and leaves the drawn point's pixel words in d4 / d5.
 * In:  script (no operands); self = a6
 * Out: d0 / d1 / d6 / d7 = the new radius / heading / x / y velocities (d0 / d1 then the drawn
 *      point's products); d4 / d5 = the drawn point swapped; the object moved; NEXT
 * Evidence: movem.l $36(a6),d0-d1/d6-d7 / add.l $46..$52(a6) / movem.l d0-d1/d6-d7,$36(a6) /
 *           add.l to $26-$32 / the VM4_CalcDrawPos code / swap d4 / swap d5; 97 script uses, e.g.
 *           $1323A (`6286 65B4`), $136E8; 137 corpus runs
 */
void Op4_Move(uint32_t *d0, uint32_t *d1, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0,
              const u16 **script, Vm4Obj *self)
{
    *d0 = (uint32_t)(int32_t)self->vel_radius + (uint32_t)(int32_t)self->acc_radius;
    *d1 = (uint32_t)(int32_t)self->vel_heading + (uint32_t)(int32_t)self->acc_heading;
    *d6 = (uint32_t)(int32_t)self->vel_x + (uint32_t)(int32_t)self->acc_x;
    *d7 = (uint32_t)(int32_t)self->vel_y + (uint32_t)(int32_t)self->acc_y;
    self->vel_radius = (int32_t)*d0;
    self->vel_heading = (int32_t)*d1;
    self->vel_x = (int32_t)*d6;
    self->vel_y = (int32_t)*d7;
    vm4_add_fix(self->radius, self->radius_frac, *d0);
    vm4_add_fix(self->heading, self->heading_frac, *d1);
    vm4_add_fix(self->x, self->x_frac, *d6);
    vm4_add_fix(self->y, self->y_frac, *d7);
    *d4 = vm4_fix(self->x, self->x_frac);
    *d5 = vm4_fix(self->y, self->y_frac);
    add_polar(d0, d1, d4, d5, a0, self);
    *d4 = swap(*d4);
    *d5 = swap(*d5);
    next(a0, script);
}

/* Op4_MovePolar - a4 op: steps only the radius and heading motion and leaves the drawn point's
 * pixel words (x / y words + radius * (cos, sin)) in d4 / d5; the fractions of x / y don't count.
 * In:  script (no operands); self = a6
 * Out: d0 = the heading velocity's high word, its low word heading & $3FE; d4 / d5 = radius *
 *      cos / sin swapped, plus the x / y words; the radius and heading moved; NEXT
 * Evidence: move.l $36(a6),d0 / add.l $46(a6),d0 / move.l d0,$36(a6) / add.l d0,$26(a6) (the same
 *           for $3A / $2A) / andi.w #$3FE / move.w $26(a6),d4 / move.w d4,d5 / muls.w / swap /
 *           add.w $2E(a6),d4 / add.w $32(a6),d5; script uses $20FF6 (`62E2 67EE`), shot scripts
 *           $50548 (`62E2 69A8`); 151 corpus runs
 */
void Op4_MovePolar(uint32_t *d0, uint32_t *d4, uint32_t *d5, const u16 **script, Vm4Obj *self)
{
    *d0 = (uint32_t)(int32_t)self->vel_radius + (uint32_t)(int32_t)self->acc_radius;
    self->vel_radius = (int32_t)*d0;
    vm4_add_fix(self->radius, self->radius_frac, *d0);
    *d0 = (uint32_t)(int32_t)self->vel_heading + (uint32_t)(int32_t)self->acc_heading;
    self->vel_heading = (int32_t)*d0;
    vm4_add_fix(self->heading, self->heading_frac, *d0);
    set_low_word(d0, (uint16_t)(self->heading & 0x3FE));
    const s16 *e = sine_entry((uint16_t)*d0);
    set_low_word(d4, (uint16_t)self->radius);
    set_low_word(d5, (uint16_t)*d4);
    *d4 = swap(muls(*d4, e[0x80]));
    *d5 = swap(muls(*d5, e[0]));
    set_low_word(d4, (uint16_t)((uint16_t)*d4 + (uint16_t)self->x));
    set_low_word(d5, (uint16_t)((uint16_t)*d5 + (uint16_t)self->y));
    uint32_t handler;
    next(&handler, script);
}

/* Op4_MoveXY - a4 op: steps only the x / y motion (velocity += acceleration, x / y += velocity)
 * and leaves the x / y pixel words in d4 / d5 (radius and heading ignored).
 * In:  script (no operands); self = a6
 * Out: d0 = the new y velocity; the object moved; then as Op4_LoadXY
 * Evidence: move.l $3E(a6),d0 / add.l $4E(a6),d0 / move.l d0,$3E(a6) / add.l d0,$2E(a6) (the same
 *           for y), falling into Op4_LoadXY; script uses $10DC6 (`632E 67EE`), $15150; 143 corpus
 *           runs
 */
void Op4_MoveXY(uint32_t *d0, uint16_t *d4, uint16_t *d5, uint32_t *a0, const u16 **script, Vm4Obj *self)
{
    *d0 = (uint32_t)(int32_t)self->vel_x + (uint32_t)(int32_t)self->acc_x;
    self->vel_x = (int32_t)*d0;
    vm4_add_fix(self->x, self->x_frac, *d0);
    *d0 = (uint32_t)(int32_t)self->vel_y + (uint32_t)(int32_t)self->acc_y;
    self->vel_y = (int32_t)*d0;
    vm4_add_fix(self->y, self->y_frac, *d0);
    Op4_LoadXY(d4, d5, a0, script, self);
}

/* Op4_LoadXY - a4 op: loads the object's x / y pixel words into d4 / d5 without moving it (a
 * still object's per-frame step before drawing).
 * In:  script (no operands); self = a6
 * Out: d4 / d5 low words = x / y; a0 = the handler; NEXT
 * Evidence: move.w $2E(a6),d4 / move.w $32(a6),d5 / NEXT; an entry inside Op4_MoveXY (computed-jump
 *           target in 151 corpus runs); 22 script uses, e.g. $108E0 (`634E 67EE`)
 */
void Op4_LoadXY(uint16_t *d4, uint16_t *d5, uint32_t *a0, const u16 **script, const Vm4Obj *self)
{
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    next(a0, script);
}

/* Op4_PolarToXY - a4 op: folds the polar part into x / y: adds radius * (cos, sin)(heading) to x /
 * y and the radius velocity's whole part * (cos, sin) to the x / y velocities, then clears the
 * radius, its velocity and its acceleration.
 * In:  script (no operands); self = a6
 * Out: d0 / d1 = the velocity products; the object; a0 = the handler; NEXT
 * Evidence: lea Tbl_059B22,a0 / andi.w #$3FE / muls.w $100(a0) / muls.w (a0) / add.l to $2E / $32
 *           and (from move.w $36(a6)) to $3E / $42 / clr.l $26 / $36 / $46; script uses $15142
 *           (`635A 6418`), $26536; 63 corpus runs
 */
void Op4_PolarToXY(uint32_t *d0, uint32_t *d1, uint32_t *a0, const u16 **script, Vm4Obj *self)
{
    const s16 *e = sine_entry((uint16_t)(self->heading & 0x3FE));
    set_low_word(d0, (uint16_t)self->radius);
    set_low_word(d1, (uint16_t)*d0);
    *d0 = muls(*d0, e[0x80]);
    *d1 = muls(*d1, e[0]);
    vm4_add_fix(self->x, self->x_frac, *d0);
    vm4_add_fix(self->y, self->y_frac, *d1);
    uint16_t speed = (uint16_t)((uint32_t)(int32_t)self->vel_radius >> 16);   /* move.w $36(a6): the whole part */
    set_low_word(d0, speed);
    set_low_word(d1, speed);
    *d0 = muls(*d0, e[0x80]);
    *d1 = muls(*d1, e[0]);
    vm4_add_long(self->vel_x, *d0);
    vm4_add_long(self->vel_y, *d1);
    vm4_set_fix(self->radius, self->radius_frac, 0);
    self->vel_radius = 0;
    self->acc_radius = 0;
    next(a0, script);
}

/* Op4_SpawnFromSelf - a4 op: loads the spawn registers d2-d5 with the object's own radius,
 * heading, x and y (16.16), so the next spawn starts on it.
 * In:  script (no operands); self = a6
 * Out: d2-d5; NEXT
 * Evidence: movem.l $26(a6),d2-d5 / NEXT; 41 script uses, e.g. $14E48 (`63A2 5606`: then
 *           Op4_Spawn); 76 corpus runs
 */
void Op4_SpawnFromSelf(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, const u16 **script,
                       const Vm4Obj *self)
{
    *d2 = vm4_fix(self->radius, self->radius_frac);
    *d3 = vm4_fix(self->heading, self->heading_frac);
    *d4 = vm4_fix(self->x, self->x_frac);
    *d5 = vm4_fix(self->y, self->y_frac);
    uint32_t handler;
    next(&handler, script);
}
