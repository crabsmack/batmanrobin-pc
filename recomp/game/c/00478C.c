#include "game.h"

/* Op2_DrawSprBack* and Op2_DrawSprAtDrawn ($00478C-$004897): draw-list ops (see 004454.c). The
 * Back forms queue their piece behind every sorted sprite (Spr_AddBack: no depth);
 * Op2_DrawSprAtDrawn draws at the object's drawn position (+$4E / +$50) instead of x / y. */

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

/* the frame word into the slot a1 names (Spr_ChangeFrame keeps its art loaded), the slot advanced */
static void change_frame(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *a0, uint32_t *a1,
                         const u16 **script)
{
    set_low_word(d1, *(*script)++);
    PTR(u16) slot;
    slot.v = *a1;
    set_low_word(d2, *slot);
    Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
    slot.v = *a1;
    *slot = (uint16_t)*d1;
    *a1 += 2;
}

/* one piece queued behind every sorted sprite (Spr_AddBack), then NEXT */
static uint32_t draw_piece_back(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t x, uint16_t y,
                                uint16_t attr, uint32_t *a0, uint32_t *a1, const u16 **script)
{
    change_frame(d0, d1, d2, d3, a0, a1, script);
    uint16_t w = (uint16_t)*d0;
    dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
    Spr_AddBack(&w, d1, x, y, attr, *a0, *a1, &spr_ccr);
    set_low_word(d0, w);
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

/* the object's position plus a byte offset pair, mirrored by the H / V flip bits (attr bits 11 /
 * 12: btst #3 / #4 of the high byte) */
static void offset_pos(uint16_t *x, uint16_t *y, const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self)
{
    uint16_t offs = *(*script)++;           /* byte dx, byte dy */
    int16_t dx = (int8_t)(offs >> 8), dy = (int8_t)(offs & 0xFF);
    if (spr->attr & 0x0800)
        dx = (int16_t)-dx;
    if (spr->attr & 0x1000)
        dy = (int16_t)-dy;
    *x = (uint16_t)(dx + self->x);
    *y = (uint16_t)(dy + self->y);
}

/* Op2_DrawSprBack - draw-list op: one frame at the object's position behind every sorted sprite.
 * In:  script = word frame; a1 = the frame slot; spr, self
 * Out: d4 / d5 = x / y, d6 = attr, d1 = the frame; the slot set, a1 + 2; Spr_ChangeFrame /
 *      Spr_AddBack's registers; handler = NEXT's
 * Evidence: move.w $38(a5),d6 / move.w $3E(a6),d4 / move.w $42(a6),d5 / move.w (a2)+,d1 /
 *           move.w (a1),d2 / bsr.w Spr_ChangeFrame / move.w d1,(a1)+ / bsr.w Spr_AddBack / NEXT;
 *           57 runs; $284FC `478C 3A20 44F2`
 */
void Op2_DrawSprBack(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                     uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                     const Vm2Obj *self)
{
    *d6 = spr->attr;
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    *a0 = draw_piece_back(d0, d1, d2, d3, *d4, *d5, *d6, a0, a1, script);
}

/* Op2_DrawSprBackAttr - draw-list op: Op2_DrawSprBack with the attribute's high byte changed:
 * (attr & mask) ^ xor.
 * In:  script = byte mask, byte xor, word frame; a1 = the frame slot
 * Out: as Op2_DrawSprBack, d6 = the changed attribute, d0's low byte = xor first
 * Evidence: and.b / eor.b on $38(a5), Spr_AddBack; 42 runs; $2FF88 `47AA 9800 4088`
 */
void Op2_DrawSprBackAttr(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                         uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                         const Vm2Obj *self)
{
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    *d6 = attr_changed(d0, script, spr);
    *a0 = draw_piece_back(d0, d1, d2, d3, *d4, *d5, *d6, a0, a1, script);
}

/* Op2_DrawSprBackOff - draw-list op: Op2_DrawSprBack at a mirrored byte offset from the object.
 * In:  script = byte dx, byte dy, word frame; a1 = the frame slot
 * Out: as Op2_DrawSprBack, d4 / d5 = the offset position
 * Evidence: byte offsets negated by the flip bits 3 / 4 of $38(a5), Spr_AddBack; 13 runs;
 *           $2F7E0 `47D0 0000 4312`. Prototype: a2 in / out (it was `a2 ... clobbers(a2)`: its
 *           NEXT hands the advanced list to the next draw op)
 */
void Op2_DrawSprBackOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                        uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                        const Vm2Obj *self)
{
    *d6 = spr->attr;
    offset_pos(d4, d5, script, spr, self);
    *a0 = draw_piece_back(d0, d1, d2, d3, *d4, *d5, *d6, a0, a1, script);
}

/* Op2_DrawSprBackAttrOff - draw-list op: Op2_DrawSprBack with the attribute change and a mirrored
 * byte offset.
 * In:  script = byte mask, byte xor, byte dx, byte dy, word frame; a1 = the frame slot
 * Out: as Op2_DrawSprBackAttr and Op2_DrawSprBackOff
 * Evidence: the Attr then Off code, Spr_AddBack; 11 runs; $2F858 `480A E810 00FF 4330`
 */
void Op2_DrawSprBackAttrOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                            uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                            const Vm2Obj *self)
{
    *d6 = attr_changed(d0, script, spr);
    offset_pos(d4, d5, script, spr, self);
    *a0 = draw_piece_back(d0, d1, d2, d3, *d4, *d5, *d6, a0, a1, script);
}

/* Op2_DrawSprAtDrawn - draw-list op: Op2_DrawSpr (sorted, the block's attribute and depth) at the
 * object's drawn position instead of x / y.
 * In:  script = word frame; a1 = the frame slot; spr, self->draw_x / draw_y
 * Out: d4 / d5 = the drawn position, d6 = attr, d7 = depth, d1 = the frame; the slot set, a1 + 2;
 *      Spr_ChangeFrame / Spr_AddSorted's registers; handler = NEXT's
 * Evidence: move.w $38(a5),d6 / move.w $3A(a5),d7 / move.w $4E(a6),d4 / move.w $50(a6),d5, then
 *           Op2_DrawSpr's tail (Spr_AddSorted); 16 runs; $19B34 `484C 31E2`
 */
void Op2_DrawSprAtDrawn(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                        uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script,
                        const Vm2Sprite *spr, const Vm2Obj *self)
{
    *d6 = spr->attr;
    *d7 = spr->depth;
    *d4 = (uint16_t)self->draw_x;
    *d5 = (uint16_t)self->draw_y;
    change_frame(d0, d1, d2, d3, a0, a1, script);
    uint16_t w = (uint16_t)*d0;
    dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
    Spr_AddSorted(&w, d1, d2, *d4, *d5, *d6, *d7, *a0, *a1, ADDR68(*script), &spr_ccr);
    set_low_word(d0, w);
    *a0 = op_next(script);
}
