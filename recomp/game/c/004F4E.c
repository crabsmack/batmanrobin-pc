#include "game.h"

/* $004F4E-$005077: the rest of the body-box ops (the off-count tail of Op2_PlaceBodyBoxes, the
 * single-box variants) and the oscillator set-up ops: Op2_SetOscX / SetOscY start one axis of
 * the oscillator block in an a6 field, Op2_AllocOsc makes sure the field holds a cleared one. */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* a .w write to a data register: the upper word kept */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* a box handle word, sign-extended (movea.w) */
static uint32_t handle_at(const s16 *h) { return (uint32_t)(int32_t)*h; }

/* a box on a collision list (prev link set) is unlinked and marked on none */
static void unlink_box(uint32_t a0, uint32_t a1)
{
    PTR(HitBox) box;
    box.v = a0;
    if (box->prev == 0)
        return;
    Hitbox_Unlink(a0, a1);
    box->prev = 0;
}

/* Op2_PlaceBodyBoxes_Off - tail of Op2_PlaceBodyBoxes while the body-box off count runs: counts it
 * down and unlinks every linked body box (the object cannot be hit).
 * In:  a1 = &vm->body[0]; d0 = body_count - 1; a4 = the VM block
 * Out: vm->body_off - 1; the boxes on no list; d0 = $FFFF (the dbra's end); a1 past the handles;
 *      a0 = the handler; NEXT
 * Evidence: subq.b #1,(a4) / movea.w (a1)+,a0 / tst.w $2(a0) / bsr.w Hitbox_Unlink / clr.w $2(a0) /
 *           dbra d0; never ran in the corpus (no writer of (a4) but Obj_SetBodyBoxes' clear found)
 */
void Op2_PlaceBodyBoxes_Off(uint16_t *d0, uint32_t *a0, uint32_t *a1, const u16 **script, Vm2Block *vm)
{
    vm->body_off = (uint8_t)(vm->body_off - 1);
    do {
        PTR(const s16) h;
        h.v = *a1;
        *a0 = handle_at(h.get());
        *a1 += 2;
        unlink_box(*a0, *a1);
    } while ((*d0)-- != 0);
    next(a0, script);
}

/* the single-box op at position (x, y): the index operand picks a body box; while the off count
 * runs the box is unlinked instead (one off the count), else Op2_PlaceBodyBoxes_Place places it
 * as a one-box loop (d0 = 0) */
static void place_one(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3,
                      uint32_t *a4, uint32_t *a5, uint32_t *a6, bool drawn)
{
    PTR(Vm2Block) vm;
    vm.v = *a4;
    PTR(const Vm2Obj) self;
    self.v = *a6;
    set_low_word(d0, *(*script)++);
    if ((int16_t)*d0 >= (int16_t)vm->body_count) {
        dk::jump(at_VM2_Next);
        return;
    }
    set_low_word(d0, (uint16_t)(*d0 * 2));
    *a0 = handle_at(&vm->body[(int16_t)*d0 / 2]);
    *d0 = 0;
    set_low_word(d1, (uint16_t)(drawn ? self->draw_x : self->x));
    set_low_word(d2, (uint16_t)(drawn ? self->draw_y : self->y));
    if (vm->body_off == 0) {
        dk::jump(at_Op2_PlaceBodyBoxes_Place);
        return;
    }
    vm->body_off = (uint8_t)(vm->body_off - 1);
    PTR(HitBox) box;
    box.v = *a0;
    if (box->prev == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    Hitbox_Unlink(*a0, *a1);
    box->prev = 0;
    next(a0, script);
    (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a3, (void)a5;
}

/* Op2_PlaceBodyBox - a2 op: places and links one body box, chosen by index, at the object's
 * position (one step of Op2_PlaceBodyBoxes); while the off count runs it unlinks that box instead.
 * In:  script = word index (out of range: nothing, VM2_Next); a4 = the VM block; a6 = the object
 * Out: a0 = the box; d0 = 0, d1 / d2 = the position; then Op2_PlaceBodyBoxes_Place's registers,
 *      or the box unlinked and NEXT
 * Evidence: move.w (a2)+,d0 / cmp.w $2(a4),d0 / bge.w VM2_Next / movea.w $4(a4,d0.w),a0 /
 *           moveq #0,d0 / tst.b (a4) / beq.w Op2_PlaceBodyBoxes_Place; 2 corpus runs; $18212
 *           `4F68 0000`
 */
void Op2_PlaceBodyBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script, uint32_t *a3,
                      uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    place_one(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, a3, a4, a5, a6, false);
}

/* Op2_PlaceBodyBoxAtDrawn - a2 op: as Op2_PlaceBodyBox at the object's drawn position.
 * In:  script = word index; a4 = the VM block; a6 = the object (draw_x / draw_y)
 * Out: as Op2_PlaceBodyBox
 * Evidence: move.w $4E(a6),d1 / move.w $50(a6),d2; 2 corpus runs; $18220 `4F9E 0001`
 */
void Op2_PlaceBodyBoxAtDrawn(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script,
                             uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    place_one(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, a3, a4, a5, a6, true);
}

/* the oscillator block in the a6 field the next operand names (move.w (a2)+,d0 / move.w (a6,d0.w),d0):
 * d0's low word = its handle, 0 = none */
static uint16_t osc_field(uint32_t *d0, const u16 **script, const Vm2Obj *self)
{
    set_low_word(d0, *(*script)++);
    set_low_word(d0, (uint16_t)*(const s16 *)((const u8 *)self + (int16_t)(uint16_t)*d0));
    return (uint16_t)*d0;
}

/* one axis started: limit = amplitude << 8 (8.8 px to 16.16), max speed = word * the speed factor
 * (unsigned), acceleration from Mot_AccelForDistance's distance and time, position and velocity 0 */
static void start_axis(OscAxis *axis, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, const u16 **script,
                       const Vm2Block *vm)
{
    *d0 = (uint32_t)((int32_t)(int16_t)(uint16_t)*(*script)++ << 8);   /* ext.l / asl.l #8 */
    axis->limit = (int32_t)*d0;
    set_low_word(d0, *(*script)++);
    *d0 = (uint32_t)(uint16_t)*d0 * (uint32_t)vm->speed;                /* mulu.w $2E(a4),d0 */
    axis->max_speed = (int32_t)*d0;
    uint16_t speed = (uint16_t)*d0;
    Mot_AccelForDistance(&speed, d1, d2, d3, script, vm);
    set_low_word(d0, speed);
    axis->accel = (int32_t)*d1;
    axis->pos = 0;
    axis->vel = 0;
}

/* Op2_SetOscX - a2 op: starts the x axis of the oscillator block in an a6 field: limit =
 * amplitude << 8, max speed = word * the speed factor, acceleration from Mot_AccelForDistance,
 * position and velocity cleared; an empty field skips the operands.
 * In:  script = word field offset, word amplitude (8.8 px), word max speed, words distance and
 *      time (Mot_AccelForDistance); vm->speed; self
 * Out: osc->x; d0-d3 as Mot_AccelForDistance leaves them (d0's upper word the max speed's);
 *      a0 = the handler; NEXT
 * Evidence: asl.l #8 -> $4(a0) / mulu.w $2E(a4) -> $C(a0) / bsr Mot_AccelForDistance -> $10(a0) /
 *           clr.l (a0), $8(a0); stepped by Op2_StepOsc; 54 corpus runs; $1EECC
 *           `4FD4 000E 0300 0080 0080 0078`
 */
void Op2_SetOscX(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint32_t *a0, const u16 **script,
                 const Vm2Block *vm, const Vm2Obj *self)
{
    uint16_t h = osc_field(d0, script, self);
    if (h == 0) {
        *script += 4;
        next(a0, script);
        return;
    }
    *a0 = (uint32_t)(int32_t)(int16_t)h;
    start_axis(&dk::handle<Vm2Osc>((int16_t)h)->x, d0, d1, d2, d3, script, vm);
    next(a0, script);
}

/* Op2_SetOscY - a2 op: as Op2_SetOscX for the y axis of the oscillator block.
 * In:  script = word field offset, word amplitude, word max speed, words distance and time
 * Out: osc->y; the registers as Op2_SetOscX; NEXT
 * Evidence: the same code on $14-$24(a0); 67 corpus runs; $133B2 `500C 000E 0200 00AA 00AA 0060`
 */
void Op2_SetOscY(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint32_t *a0, const u16 **script,
                 const Vm2Block *vm, const Vm2Obj *self)
{
    uint16_t h = osc_field(d0, script, self);
    if (h == 0) {
        *script += 4;
        next(a0, script);
        return;
    }
    *a0 = (uint32_t)(int32_t)(int16_t)h;
    start_axis(&dk::handle<Vm2Osc>((int16_t)h)->y, d0, d1, d2, d3, script, vm);
    next(a0, script);
}

/* Op2_AllocOsc - a2 op: makes sure an a6 field holds an oscillator block (allocates a pool block
 * when it is 0; with the pool empty it just continues, VM2_Next) and clears its 40 bytes.
 * In:  script = word field offset; self
 * Out: d1 = the offset; the field = the block; the block cleared; d0 = 0; a0 = the handler; NEXT
 * Evidence: move.w (a2)+,d1 / move.w (a6,d1.w),d0 / beq / bsr ObjPool_Alloc / bne.w VM2_Next /
 *           move.w a0,(a6,d1.w) / moveq #0,d0 / 10x move.l d0,(a0)+; 70 corpus runs; $133AE
 *           `5046 000E 500C 000E`
 */
void Op2_AllocOsc(uint32_t *d0, uint16_t *d1, uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    *d1 = *(*script)++;
    s16 *field = (s16 *)((u8 *)self + (int16_t)*d1);
    set_low_word(d0, (uint16_t)*field);
    if ((uint16_t)*d0 != 0) {
        *a0 = (uint32_t)(int32_t)(int16_t)(uint16_t)*d0;
    } else {
        ObjPool_Alloc(d0, a0);
        if (*d0 != 0) {
            dk::jump(at_VM2_Next);
            return;
        }
        *field = (int16_t)(uint16_t)*a0;
    }
    *d0 = 0;
    PTR(Vm2Osc) osc;
    osc.v = *a0;
    dk::mem_set(osc.get(), 0, sizeof(Vm2Osc));   /* the 10 move.l d0,(a0)+ */
    next(a0, script);
}
