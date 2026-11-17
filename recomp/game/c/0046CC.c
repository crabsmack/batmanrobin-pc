#include "game.h"

/* Op2_DrawSprFront* ($0046CC-$00474B): draw-list ops (see 004454.c) that queue their piece in front
 * of every sorted sprite (Spr_AddFront: no depth). */

/* NEXT: the handler the script's next word names, jumped to */
static uint32_t op_next(const u16 **script)
{
    uint32_t handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(handler);
    return handler;
}

/* the low word / byte of a data register, the rest kept */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void set_low_byte(uint32_t *r, uint8_t v) { *r = (*r & 0xFFFFFF00u) | v; }

/* one piece: the frame word into the slot a1 names (Spr_ChangeFrame), the slot advanced, the
 * piece queued in front at (x, y) with attr (Spr_AddFront), NEXT */
static uint32_t draw_piece_front(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t x, uint16_t y,
                                 uint16_t attr, uint32_t *a0, uint32_t *a1, const u16 **script)
{
    set_low_word(d1, *(*script)++);
    PTR(u16) slot;
    slot.v = *a1;
    set_low_word(d2, *slot);
    Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
    slot.v = *a1;
    *slot = (uint16_t)*d1;
    *a1 += 2;
    uint16_t frame = (uint16_t)*d1;
    dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
    Spr_AddFront(d0, &frame, x, y, attr, a0, &spr_ccr);
    set_low_word(d1, frame);
    return op_next(script);
}

/* the attribute's high byte changed: (byte & mask) ^ xor, in the high byte (lsl.w #8); d0's low
 * byte = xor (move.b (a2)+,d0) */
static uint16_t attr_changed(uint32_t *d0, const u16 **script, const Vm2Sprite *spr)
{
    uint16_t ops = *(*script)++;            /* byte mask, byte xor */
    set_low_byte(d0, (uint8_t)ops);
    return (uint16_t)((((spr->attr >> 8) & (ops >> 8)) ^ (ops & 0xFF)) << 8);
}

/* Op2_DrawSprFront - draw-list op: one frame at the object's position in front of every sorted
 * sprite.
 * In:  script = word frame; a1 = the frame slot; spr, self
 * Out: d4 / d5 = x / y, d6 = attr, d1 = the frame; the slot set, a1 + 2; Spr_ChangeFrame /
 *      Spr_AddFront's registers; handler = NEXT's
 * Evidence: move.w $38(a5),d6 / move.w $3E(a6),d4 / move.w $42(a6),d5 / move.w (a2)+,d1 /
 *           move.w (a1),d2 / bsr.w Spr_ChangeFrame / move.w d1,(a1)+ / bsr.w Spr_AddFront / NEXT;
 *           51 runs; $1E094 `46CC 32E6 44F2`
 */
void Op2_DrawSprFront(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                      uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                      const Vm2Obj *self)
{
    *d6 = spr->attr;
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    *a0 = draw_piece_front(d0, d1, d2, d3, *d4, *d5, *d6, a0, a1, script);
}

/* Op2_DrawSprFrontAttr - draw-list op: Op2_DrawSprFront with the attribute's high byte changed:
 * (attr & mask) ^ xor.
 * In:  script = byte mask, byte xor, word frame; a1 = the frame slot
 * Out: as Op2_DrawSprFront, d6 = the changed attribute, d0's low byte = xor first
 * Evidence: move.b $38(a5),d6 / and.b (a2)+,d6 / move.b (a2)+,d0 / eor.b d0,d6 / lsl.w #8,d6,
 *           Spr_AddFront; 30 runs; $28978 `46EA F810 3ACA`. Prototype: a2 in / out (it was
 *           `a2 ... clobbers(a2)`: its NEXT hands the advanced list to the next draw op)
 */
void Op2_DrawSprFrontAttr(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                          uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                          const Vm2Obj *self)
{
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    *d6 = attr_changed(d0, script, spr);
    *a0 = draw_piece_front(d0, d1, d2, d3, *d4, *d5, *d6, a0, a1, script);
}

/* Op2_DrawSprFrontOff - draw-list op: Op2_DrawSprFront at a signed byte offset from the object,
 * mirrored by the attribute's H / V flip bits (11 / 12).
 * In:  script = byte dx, byte dy, word frame; a1 = the frame slot
 * Out: as Op2_DrawSprFront, d4 / d5 = the offset position
 * Evidence: move.b (a2)+,d4 / move.b (a2)+,d5 / ext.w / btst #3 / #4,$38(a5) + neg.w /
 *           add.w $3E(a6),d4 / add.w $42(a6),d5, Spr_AddFront; 10 runs; $207DE `4710 000A 2CEC`.
 *           Prototype: a2 in / out (as Op2_DrawSprFrontAttr)
 */
void Op2_DrawSprFrontOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                         uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                         const Vm2Obj *self)
{
    *d6 = spr->attr;
    uint16_t offs = *(*script)++;           /* byte dx, byte dy */
    int16_t dx = (int8_t)(offs >> 8), dy = (int8_t)(offs & 0xFF);
    if (spr->attr & 0x0800)
        dx = (int16_t)-dx;
    if (spr->attr & 0x1000)
        dy = (int16_t)-dy;
    *d4 = (uint16_t)(dx + self->x);
    *d5 = (uint16_t)(dy + self->y);
    *a0 = draw_piece_front(d0, d1, d2, d3, *d4, *d5, *d6, a0, a1, script);
}
