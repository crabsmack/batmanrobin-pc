#include "game.h"

/* Op2_StopOscY, the oscillator step ops and the relative effect-spawn ops ($0050CE-$0051F1). */

/* NEXT: the handler the script's next word names, jumped to */
static void op_next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the oscillator handle in the a6 field a byte offset names (move.w (a6,d0.w),d0) */
static uint16_t osc_field(const Vm2Obj *self, uint16_t offset)
{
    return (uint16_t)*(const s16 *)((const u8 *)self + (int16_t)offset);
}

/* One axis of an oscillator step: the velocity accelerates toward the signed max speed (clamped
 * to it), the position moves by it, and when the position reaches the limit (from below for a
 * positive max speed, from above for a negative one) limit, max speed and acceleration change
 * sign, so the axis swings back. d0/d4-d7 = the axis's five longs as stored back; returns the
 * velocity (this run's move). */
static uint32_t step_axis(OscAxis *axis, uint32_t *d0, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7)
{
    *d0 = (uint32_t)(int32_t)axis->pos;
    *d4 = (uint32_t)(int32_t)axis->limit;
    *d5 = (uint32_t)(int32_t)axis->vel;
    *d6 = (uint32_t)(int32_t)axis->max_speed;
    *d7 = (uint32_t)(int32_t)axis->accel;
    bool turn;
    *d5 += *d7;
    if ((int32_t)*d6 >= 0) {
        if ((int32_t)*d5 > (int32_t)*d6)
            *d5 = *d6;
        *d0 += *d5;
        turn = (int32_t)*d0 >= (int32_t)*d4;
    } else {
        if ((int32_t)*d5 < (int32_t)*d6)
            *d5 = *d6;
        *d0 += *d5;
        turn = (int32_t)*d0 <= (int32_t)*d4;
    }
    if (turn) {
        *d4 = 0u - *d4;
        *d6 = 0u - *d6;
        *d7 = 0u - *d7;
    }
    axis->pos = (int32_t)*d0;
    axis->limit = (int32_t)*d4;
    axis->vel = (int32_t)*d5;
    axis->max_speed = (int32_t)*d6;
    axis->accel = (int32_t)*d7;
    return *d5;
}

/* Op2_StopOscY - a2 op: stops the y axis of the oscillator block in the a6 field the operand
 * names (its max speed = 0); an empty field does nothing.
 * In:  script = word field offset; self
 * Out: d0 = the handle; y.max_speed = 0; a0 = the next op's handler (NEXT)
 * Evidence: move.w (a2)+,d0 / move.w (a6,d0.w),d0 / beq VM2_Next / movea.w d0,a0 / clr.l $20(a0) /
 *           NEXT; 3 corpus runs; $3BA88 `50CE 000E`
 */
void Op2_StopOscY(uint16_t *d0, uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    *d0 = osc_field(self, *(*script)++);
    if (*d0 != 0)
        dk::handle<Vm2Osc>((int16_t)*d0)->y.max_speed = 0;
    op_next(a0, script);
}

/* Op2_StepOsc - a2 op: steps the oscillator block in the a6 field the operand names, its two
 * velocities becoming this run's move d1/d2 (0 for an empty field).
 * In:  script = word field offset; self
 * Out: d1/d2 = 0, then as Op2_AddOsc
 * Evidence: moveq #0,d1 / moveq #0,d2, falling into Op2_AddOsc; 27 corpus runs; $3B432 `50E2 0008 4C52`
 */
void Op2_StepOsc(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7,
                 uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    *d1 = 0;
    *d2 = 0;
    Op2_AddOsc(d0, d1, d2, d4, d5, d6, d7, a0, script, self);
}

/* Op2_AddOsc - a2 op: as Op2_StepOsc, but the oscillator's velocities are added to the move d1/d2
 * already computed (Op2_StepVelocity), so the swing rides on the normal motion.
 * In:  script = word field offset; d1/d2 = the move so far; self
 * Out: d1/d2 += the x/y velocities; d0/d4-d7 = the y axis's longs (d0 low word = 0 for an empty
 *      field); a0 = the next op's handler (NEXT)
 * Evidence: move.w (a2)+,d0 / move.w (a6,d0.w),d0 / beq VM2_Next / movea.w d0,a0 / movem.l (a0),d0/d4-d7
 *           ... movem.l d0/d4-d7,(a0) / the same at $14(a0) / NEXT; 92 corpus runs; $133C0 `4B74 50E6
 *           000E 4CDA`
 */
void Op2_AddOsc(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7,
                uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    uint16_t handle = osc_field(self, *(*script)++);
    *d0 = (*d0 & 0xFFFF0000u) | handle;
    if (handle != 0) {
        Vm2Osc *osc = dk::handle<Vm2Osc>((int16_t)handle);
        *d1 += step_axis(&osc->x, d0, d4, d5, d6, d7);
        *d2 += step_axis(&osc->y, d0, d4, d5, d6, d7);
    }
    op_next(a0, script);
}

/* The effect-spawn tail ($519E, and $5210's with an absolute script): runs the a4 script at
 * `a4_script` on g_SpawnCtx (velocity = d6/d7, radius and heading speeds 0, attributes and depth
 * from the sprite block) with the spawn registers d2-d5 (d4/d5 swapped: pixel words high), its
 * first op called; a1-a6 saved around it on the stack (movem.l a1-a6), then NEXT. */
static void run_spawn_script(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script,
                             const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self, uint32_t a4_script)
{
    uint32_t r_a1 = a1, r_a2 = dk::addr(*script), r_a3 = dk::addr(mot), r_a5 = dk::addr(spr), r_a6 = dk::addr(self);
    dk::push_long(r_a6);
    dk::push_long(r_a5);
    dk::push_long(a4);
    dk::push_long(r_a3);
    dk::push_long(r_a2);
    dk::push_long(r_a1);
    Vm4Obj *ctx = (Vm4Obj *)g_SpawnCtx;
    ctx->vel_x = (int32_t)*d6;
    ctx->vel_y = (int32_t)*d7;
    ctx->vel_radius = 0;
    ctx->vel_heading = 0;
    ctx->attr = (uint16_t)spr->attr;
    ctx->depth = (uint16_t)spr->depth;
    r_a6 = dk::addr(ctx);
    PTR(const u16) first;
    first.v = a4_script;
    *a0 = dk::code((int16_t)(uint16_t)*first++);
    uint32_t r_a4 = first.raw();
    *d4 = *d4 << 16 | *d4 >> 16;
    *d5 = *d5 << 16 | *d5 >> 16;
    Op4CodeFn(*a0, d0, d1, d2, d3, d4, d5, d6, d7, a0, &r_a1, &r_a2, &r_a3, &r_a4, &r_a5, &r_a6);
    (void)dk::pop_long();                    /* movem.l (a7)+,a1-a6: only a2 is handed on */
    PTR(const u16) resume;
    resume.v = dk::pop_long();
    for (int i = 0; i < 4; i++)
        (void)dk::pop_long();
    *script = resume;
    op_next(a0, script);
}

/* the relative script of the spawn ops: the offset word's address plus its value (move.w (a2)+,d0
 * / lea -2(a2,d0.w),a4) */
static uint32_t rel_script(uint32_t *d0, const u16 **script)
{
    uint32_t word_at = dk::addr(*script);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)*(*script)++;
    return word_at + (uint32_t)(int32_t)(int16_t)(uint16_t)*d0;
}

/* the byte offset pair of Op2_FxSpawnOff: d4/d5 low words = the object's x/y plus the signed bytes
   (move.b / ext.w / add.w: the upper words kept) */
static void byte_offset_pos(uint32_t *d4, uint32_t *d5, const u16 **script, const Vm2Obj *self)
{
    const s8 *bytes = (const s8 *)*script;
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)((uint16_t)(int16_t)bytes[0] + (uint16_t)self->x);
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)((uint16_t)(int16_t)bytes[1] + (uint16_t)self->y);
    (*script)++;
}

/* Op2_FxSpawnRegsImpulse34 - a2 op: runs an a4 effect spawn script on g_SpawnCtx with the spawn
 * registers the Op2_FxPos* / Op2_FxHeading* ops left and 3/4 of the motion block's impulse as the
 * spawn velocity.
 * In:  script = word offset to the a4 script; d2 radius, d3 heading (high word), d4/d5 x/y words;
 *      mot, spr, self
 * Out: d6/d7 = the 3/4 impulse; d0 = the offset word (upper word: impulse y / 4); what the spawn
 *      script left in d0-d7/a0; NEXT
 * Evidence: movem.l $2(a3),d6-d7 / move.l d6,d0 / asr.l #2,d0 / sub.l d0,d6 / the same for d7 /
 *           bra $519E; 20 corpus runs; $2065C `515E 0012`
 */
void Op2_FxSpawnRegsImpulse34(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script,
                              const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self)
{
    *d6 = (uint32_t)(int32_t)mot->impulse_x;
    *d7 = (uint32_t)(int32_t)mot->impulse_y;
    *d0 = (uint32_t)((int32_t)*d6 >> 2);
    *d6 -= *d0;
    *d0 = (uint32_t)((int32_t)*d7 >> 2);
    *d7 -= *d0;
    uint32_t a4_script = rel_script(d0, script);
    run_spawn_script(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, mot, a4, spr, self, a4_script);
}

/* Op2_FxSpawnRegsImpulse - a2 op: as Op2_FxSpawnRegs but the spawn velocity is the motion block's
 * impulse.
 * In:  as Op2_FxSpawnRegsImpulse34
 * Out: d6/d7 = the impulse; as Op2_FxSpawnRegs
 * Evidence: movem.l $2(a3),d6-d7 / bra $519E; 30 corpus runs; $1AFE8 `5264 5172 05E4`
 */
void Op2_FxSpawnRegsImpulse(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script,
                            const Vm2Motion *mot, uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self)
{
    *d6 = (uint32_t)(int32_t)mot->impulse_x;
    *d7 = (uint32_t)(int32_t)mot->impulse_y;
    uint32_t a4_script = rel_script(d0, script);
    run_spawn_script(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, mot, a4, spr, self, a4_script);
}

/* Op2_FxSpawnOff - a2 op: runs an a4 effect spawn script on g_SpawnCtx at the object's position
 * plus a signed byte offset (not mirrored), radius and heading 0, with the object's velocity.
 * In:  script = byte dx, byte dy, word offset to the a4 script; mot, spr, self
 * Out: d4/d5 low words = the position (then swapped by the spawn); d2 = d3 = 0; as Op2_FxSpawnRegs
 * Evidence: move.b (a2)+,d4/d5 / ext.w / add.w $3E/$42(a6) / bra $5194 (moveq #0,d2/d3, then
 *           Op2_FxSpawnRegs); 42 corpus runs, 62 script uses (e.g. $19666 `517A 0039 156C`)
 */
void Op2_FxSpawnOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                    uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot,
                    uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self)
{
    byte_offset_pos(d4, d5, script, self);
    *d2 = 0;
    *d3 = 0;
    *d6 = (uint32_t)(int32_t)mot->vel_x;
    *d7 = (uint32_t)(int32_t)mot->vel_y;
    uint32_t a4_script = rel_script(d0, script);
    run_spawn_script(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, mot, a4, spr, self, a4_script);
}

/* Op2_FxSpawnHere - a2 op: runs an a4 effect spawn script on g_SpawnCtx at the object's position,
 * radius and heading 0, with the object's velocity.
 * In:  script = word offset to the a4 script; mot, spr, self
 * Out: d4/d5 low words = the object's x/y (then swapped); d2 = d3 = 0; as Op2_FxSpawnRegs
 * Evidence: move.w $3E(a6),d4 / move.w $42(a6),d5 / moveq #0,d2 / moveq #0,d3, falling into
 *           Op2_FxSpawnRegs; 64 corpus runs; $1998E `518C 10C6`
 */
void Op2_FxSpawnHere(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                     uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot,
                     uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self)
{
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)self->x;
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)self->y;
    *d2 = 0;
    *d3 = 0;
    *d6 = (uint32_t)(int32_t)mot->vel_x;
    *d7 = (uint32_t)(int32_t)mot->vel_y;
    uint32_t a4_script = rel_script(d0, script);
    run_spawn_script(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, mot, a4, spr, self, a4_script);
}

/* Op2_FxSpawnRegs - a2 op: runs an a4 effect spawn script on g_SpawnCtx with the spawn registers
 * the Op2_FxPos* / Op2_FxHeading* ops prepared, with the object's velocity (motion block +$18/$1C).
 * In:  script = word offset to the a4 script; d2 radius, d3 heading (high word), d4/d5 x/y words;
 *      mot, spr, self
 * Out: d6/d7 = the velocity; d0 = the offset word; d4/d5 swapped; what the spawn script left in
 *      d0-d7/a0; a2 past the operand (restored from the stack); NEXT
 * Evidence: movem.l $18(a3),d6-d7 / move.w (a2)+,d0 / movem.l a1-a6,-(a7) / lea g_SpawnCtx,a6 /
 *           movem.l d6-d7,$3E(a6) / clr.l $36(a6) / clr.l $3A(a6) / move.w $38(a5),$22(a6) / move.w
 *           $3A(a5),$24(a6) / lea -2(a2,d0.w),a4 / movea.w (a4)+,a0 / swap d4 / swap d5 / jsr (a0) /
 *           movem.l (a7)+,a1-a6 / NEXT; 127 corpus runs, 155 script uses
 */
void Op2_FxSpawnRegs(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                     uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot,
                     uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self)
{
    *d6 = (uint32_t)(int32_t)mot->vel_x;
    *d7 = (uint32_t)(int32_t)mot->vel_y;
    uint32_t a4_script = rel_script(d0, script);
    run_spawn_script(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, mot, a4, spr, self, a4_script);
}
