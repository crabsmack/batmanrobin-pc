#include "game.h"

/* The absolute effect-spawn ops and the spawn-position ops ($0051F2-$0052CD): Op2_FxSpawn*Abs
 * take a dc.l a4 script; Op2_FxPos* set or move the spawn position registers d4/d5 (pixel words)
 * and clear radius d2 / heading d3 for a later Op2_FxSpawnRegs*. */

/* NEXT: the handler the script's next word names, jumped to */
static void op_next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the next handler word, for the ops whose a0 nobody reads */
static uint32_t next_op(const u16 **script)
{
    return dk::code((int16_t)(uint16_t)*(*script)++);
}

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* The effect-spawn tail ($5210's): runs the a4 script at `a4_script` on g_SpawnCtx (velocity =
 * d6/d7, radius and heading speeds 0, attributes and depth from the sprite block) with the spawn
 * registers d2-d5 (d4/d5 swapped: pixel words high), its first op called; a1-a6 saved around it
 * on the stack (movem.l a1-a6), then NEXT. */
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

/* the absolute a4 script operand (move.l (a2)+,d0 / movea.l d0,a4), after the velocity load */
static void spawn_abs(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                      uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot,
                      uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self)
{
    *d6 = (uint32_t)(int32_t)mot->vel_x;
    *d7 = (uint32_t)(int32_t)mot->vel_y;
    *d0 = (uint32_t)(uint16_t)(*script)[0] << 16 | (uint16_t)(*script)[1];
    *script += 2;
    run_spawn_script(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, mot, a4, spr, self, *d0);
}

/* Op2_FxSpawnOffAbs - a2 op: as Op2_FxSpawnOff with an absolute (dc.l) a4 script address.
 * In:  script = byte dx, byte dy, long a4 script; mot, spr, self
 * Out: d4/d5 low words = the object's x/y plus the signed bytes (then swapped); d2 = d3 = 0; as
 *      Op2_FxSpawnRegsAbs
 * Evidence: move.b (a2)+,d4/d5 / ext.w / add.w $3E/$42(a6) / bra $520C (moveq #0,d2/d3, then
 *           Op2_FxSpawnRegsAbs); 22 corpus runs; $254B2 `51F2 FE08 0001 0D9A`
 */
void Op2_FxSpawnOffAbs(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot,
                       uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self)
{
    const s8 *bytes = (const s8 *)*script;
    set_low_word(d4, (uint16_t)((uint16_t)(int16_t)bytes[0] + (uint16_t)self->x));
    set_low_word(d5, (uint16_t)((uint16_t)(int16_t)bytes[1] + (uint16_t)self->y));
    (*script)++;
    *d2 = 0;
    *d3 = 0;
    spawn_abs(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, mot, a4, spr, self);
}

/* Op2_FxSpawnHereAbs - a2 op: as Op2_FxSpawnHere with an absolute (dc.l) a4 script address.
 * In:  script = long a4 script; mot, spr, self
 * Out: d4/d5 low words = the object's x/y (then swapped); d2 = d3 = 0; as Op2_FxSpawnRegsAbs
 * Evidence: move.w $3E(a6),d4 / move.w $42(a6),d5 / moveq #0,d2 / moveq #0,d3, falling into
 *           Op2_FxSpawnRegsAbs; 80 corpus runs; $1E078 `5204 0001 4C7E`
 */
void Op2_FxSpawnHereAbs(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot,
                        uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self)
{
    set_low_word(d4, (uint16_t)self->x);
    set_low_word(d5, (uint16_t)self->y);
    *d2 = 0;
    *d3 = 0;
    spawn_abs(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, mot, a4, spr, self);
}

/* Op2_FxSpawnRegsAbs - a2 op: as Op2_FxSpawnRegs with an absolute (dc.l) a4 script address.
 * In:  script = long a4 script; d2 radius, d3 heading (high word), d4/d5 x/y words; mot, spr, self
 * Out: d6/d7 = the object's velocity; d0 = the script address; d4/d5 swapped; what the spawn script
 *      left in d0-d7/a0; a2 past the operand (restored from the stack); NEXT
 * Evidence: movem.l $18(a3),d6-d7 / move.l (a2)+,d0 / movem.l a1-a6,-(a7) / lea g_SpawnCtx,a6 / ... /
 *           movea.l d0,a4 / movea.w (a4)+,a0 / swap d4 / swap d5 / jsr (a0) / movem.l (a7)+,a1-a6 /
 *           NEXT; 11 corpus runs; $37A42 `5210 0001 0D9A 5284 5294 0200 5210`
 */
void Op2_FxSpawnRegsAbs(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, const u16 **script, const Vm2Motion *mot,
                        uint32_t a4, const Vm2Sprite *spr, Vm2Obj *self)
{
    spawn_abs(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, script, mot, a4, spr, self);
}

/* Op2_FxPosScreenCentre - a2 op: sets the spawn position to the screen centre (camera x + $A0,
 * camera y + $70), radius and heading 0.
 * In:  g_CameraX, g_CameraY (their integer words)
 * Out: d4/d5 = the centre; d2 = d3 = 0; NEXT
 * Evidence: move.w (g_CameraX).w,d4 / move.w (g_CameraY).w,d5 / addi.w #$A0,d4 / addi.w #$70,d5 /
 *           moveq #0,d2 / moveq #0,d3; 4 corpus runs; $1E848 `524C 5294 0092 5198`
 */
void Op2_FxPosScreenCentre(uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, const u16 **script)
{
    *d4 = (uint16_t)((uint16_t)(g_CameraX >> 16) + 0xA0);
    *d5 = (uint16_t)((uint16_t)(g_CameraY >> 16) + 0x70);
    *d2 = 0;
    *d3 = 0;
    dk::jump(next_op(script));
}

/* Op2_FxPosHere - a2 op: sets the spawn position to the object's position, radius and heading 0.
 * In:  self
 * Out: d4/d5 = its x/y; d2 = d3 = 0; NEXT
 * Evidence: move.w $3E(a6),d4 / move.w $42(a6),d5 / moveq #0,d2/d3; 109 corpus runs, 154 script uses
 */
void Op2_FxPosHere(uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, const u16 **script, const Vm2Obj *self)
{
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    *d2 = 0;
    *d3 = 0;
    dk::jump(next_op(script));
}

/* Op2_FxPosAnchor - a2 op: sets the spawn position to the object's anchor (its saved position),
 * radius and heading 0.
 * In:  self (saved_x / saved_y: their integer words)
 * Out: d4/d5 = the anchor; d2 = d3 = 0; NEXT
 * Evidence: move.w $46(a6),d4 / move.w $4A(a6),d5 / moveq #0,d2/d3; 27 corpus runs; $37A02 `5274 5198 F582`
 */
void Op2_FxPosAnchor(uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, const u16 **script, const Vm2Obj *self)
{
    *d4 = (uint16_t)((uint32_t)(int32_t)self->saved_x >> 16);
    *d5 = (uint16_t)((uint32_t)(int32_t)self->saved_y >> 16);
    *d2 = 0;
    *d3 = 0;
    dk::jump(next_op(script));
}

/* Op2_FxPosDrawn - a2 op: sets the spawn position to the object's drawn position, radius and
 * heading 0.
 * In:  self (draw_x / draw_y)
 * Out: d4/d5 = the drawn position; d2 = d3 = 0; NEXT
 * Evidence: move.w $4E(a6),d4 / move.w $50(a6),d5 / moveq #0,d2/d3; 16 corpus runs; $19A30 `5284 5198 0B64`
 */
void Op2_FxPosDrawn(uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, const u16 **script, const Vm2Obj *self)
{
    *d4 = (uint16_t)self->draw_x;
    *d5 = (uint16_t)self->draw_y;
    *d2 = 0;
    *d3 = 0;
    dk::jump(next_op(script));
}

/* Op2_FxPosAddB - a2 op: adds a signed byte offset to the spawn position (not mirrored).
 * In:  script = byte dx, byte dy; d4/d5 = the position
 * Out: d1 = dy (extended); d4/d5 moved; NEXT
 * Evidence: move.b (a2)+,d0 / move.b (a2)+,d1 / ext.w d0 / ext.w d1 / add.w d0,d4 / add.w d1,d5; 33
 *           corpus runs; $1C2C6 `5294 F6E3`
 */
void Op2_FxPosAddB(uint16_t *d1, uint16_t *d4, uint16_t *d5, const u16 **script)
{
    const s8 *bytes = (const s8 *)*script;
    uint16_t dx = (uint16_t)(int16_t)bytes[0];
    *d1 = (uint16_t)(int16_t)bytes[1];
    (*script)++;
    *d4 = (uint16_t)(*d4 + dx);
    *d5 = (uint16_t)(*d5 + *d1);
    dk::jump(next_op(script));
}

/* Op2_FxPosAddW - a2 op: adds a signed word offset to the spawn position.
 * In:  script = word dx, word dy; d4/d5 = the position
 * Out: d4/d5 moved; NEXT
 * Evidence: add.w (a2)+,d4 / add.w (a2)+,d5; 3 corpus runs; $25DA4 `52A4 0100 0060`
 */
void Op2_FxPosAddW(uint16_t *d4, uint16_t *d5, const u16 **script)
{
    *d4 = (uint16_t)(*d4 + *(*script)++);
    *d5 = (uint16_t)(*d5 + *(*script)++);
    dk::jump(next_op(script));
}

/* sprite attribute bits 11/12: the sprite is mirrored horizontally / vertically */
static constexpr uint16_t FX_ATTR_FLIP_H = 0x0800;
static constexpr uint16_t FX_ATTR_FLIP_V = 0x1000;

/* Op2_FxPosAddFlip - a2 op: adds a signed byte offset, mirrored by the sprite's flip bits, to the
 * spawn position.
 * In:  script = byte dx, byte dy; d4/d5 = the position; spr (attr)
 * Out: d1 = dy (extended, mirrored); d4/d5 moved; NEXT
 * Evidence: move.b (a2)+,d0/d1 / ext.w / btst #3,$38(a5) / neg.w d0 / btst #4,$38(a5) / neg.w d1 /
 *           add.w d0,d4 / add.w d1,d5; 71 corpus runs; $1B442 `52AC 08F8`
 */
void Op2_FxPosAddFlip(uint16_t *d1, uint16_t *d4, uint16_t *d5, const u16 **script, const Vm2Sprite *spr)
{
    const s8 *bytes = (const s8 *)*script;
    uint16_t dx = (uint16_t)(int16_t)bytes[0];
    *d1 = (uint16_t)(int16_t)bytes[1];
    (*script)++;
    if (spr->attr & FX_ATTR_FLIP_H)
        dx = (uint16_t)-dx;
    if (spr->attr & FX_ATTR_FLIP_V)
        *d1 = (uint16_t)-*d1;
    *d4 = (uint16_t)(*d4 + dx);
    *d5 = (uint16_t)(*d5 + *d1);
    dk::jump(next_op(script));
}
