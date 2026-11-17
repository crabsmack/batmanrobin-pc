#include "game.h"

/* Op2_DrawSprAtDrawnDepth / AtDrawnAttrDepth / AtDrawnOff ($004898-$004927): draw-list ops (see
 * 004454.c) drawing a sorted piece at the object's drawn position (+$4E / +$50). */

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

/* one sorted piece: the frame word into the slot a1 names (Spr_ChangeFrame), the slot advanced,
 * the piece queued at (x, y) with attr / depth (Spr_AddSorted), NEXT */
static uint32_t draw_piece(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t x, uint16_t y,
                           uint16_t attr, uint16_t depth, uint32_t *a0, uint32_t *a1, const u16 **script)
{
    set_low_word(d1, *(*script)++);
    PTR(u16) slot;
    slot.v = *a1;
    set_low_word(d2, *slot);
    Spr_ChangeFrame(d0, d1, d2, d3, a0, a1);
    slot.v = *a1;
    *slot = (uint16_t)*d1;
    *a1 += 2;
    uint16_t w = (uint16_t)*d0;
    dk::Flags spr_ccr{};   /* its &ccr (Spr_* hand their condition codes back) */
    Spr_AddSorted(&w, d1, d2, x, y, attr, depth, *a0, *a1, ADDR68(*script), &spr_ccr);
    set_low_word(d0, w);
    return op_next(script);
}

/* Op2_DrawSprAtDrawnDepth - draw-list op: Op2_DrawSprAtDrawn with a depth offset.
 * In:  script = word depth offset, word frame; a1 = the frame slot; spr, self->draw_x / draw_y
 * Out: d4 / d5 = the drawn position, d6 = attr, d7 = depth + offset, d1 = the frame; a1 + 2; NEXT
 * Evidence: move.w $4E(a6),d4 / move.w $50(a6),d5 / move.w $3A(a5),d7 / add.w (a2)+,d7, then
 *           Spr_ChangeFrame / Spr_AddSorted; 2 runs; $19C74 `4898 FFF6 31D8`
 */
void Op2_DrawSprAtDrawnDepth(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                             uint16_t *d6, uint16_t *d7, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                             const Vm2Obj *self)
{
    *d6 = spr->attr;
    *d4 = (uint16_t)self->draw_x;
    *d5 = (uint16_t)self->draw_y;
    *d7 = (uint16_t)(spr->depth + *(*script)++);
    uint32_t a0 = 0;
    draw_piece(d0, d1, d2, d3, *d4, *d5, *d6, *d7, &a0, a1, script);
}

/* Op2_DrawSprAtDrawnAttrDepth - draw-list op: Op2_DrawSprAtDrawn with the attribute's high byte
 * changed ((attr & mask) ^ xor) and a depth offset.
 * In:  script = byte mask, byte xor, word depth offset, word frame; a1 = the frame slot
 * Out: as Op2_DrawSprAtDrawnDepth, d6 = the changed attribute, d0's low byte = xor first;
 *      handler = NEXT's
 * Evidence: $4E/$50(a6), and.b / eor.b on $38(a5), add.w (a2)+,d7; 2 runs; $19C7E `48BC F000 FFF6 FD30`
 */
void Op2_DrawSprAtDrawnAttrDepth(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4,
                                 uint16_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1,
                                 const u16 **script, const Vm2Sprite *spr, const Vm2Obj *self)
{
    *d4 = (uint16_t)self->draw_x;
    *d5 = (uint16_t)self->draw_y;
    uint16_t ops = *(*script)++;            /* byte mask, byte xor */
    set_low_byte(d0, (uint8_t)ops);
    *d6 = (uint16_t)((((spr->attr >> 8) & (ops >> 8)) ^ (ops & 0xFF)) << 8);
    *d7 = (uint16_t)(spr->depth + *(*script)++);
    *a0 = draw_piece(d0, d1, d2, d3, *d4, *d5, *d6, *d7, a0, a1, script);
}

/* Op2_DrawSprAtDrawnOff - draw-list op: Op2_DrawSprAtDrawn plus a signed byte offset, mirrored
 * by the attribute's H / V flip bits (11 / 12).
 * In:  script = byte dx, byte dy, word frame; a1 = the frame slot
 * Out: d4 / d5 = the offset drawn position, d6 = attr, d1 = the frame; a1 + 2; handler = NEXT's
 *      (d7 = depth, a clobber)
 * Evidence: move.b (a2)+,d4 / move.b (a2)+,d5 / ext.w / btst #3 / #4,$38(a5) + neg.w /
 *           add.w $4E(a6),d4 / add.w $50(a6),d5, Spr_AddSorted; 12 runs
 */
void Op2_DrawSprAtDrawnOff(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                           uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                           const Vm2Obj *self)
{
    *d6 = spr->attr;
    uint16_t depth = spr->depth;
    uint16_t offs = *(*script)++;           /* byte dx, byte dy */
    int16_t dx = (int8_t)(offs >> 8), dy = (int8_t)(offs & 0xFF);
    if (spr->attr & 0x0800)
        dx = (int16_t)-dx;
    if (spr->attr & 0x1000)
        dy = (int16_t)-dy;
    *d4 = (uint16_t)(dx + self->draw_x);
    *d5 = (uint16_t)(dy + self->draw_y);
    *a0 = draw_piece(d0, d1, d2, d3, *d4, *d5, *d6, depth, a0, a1, script);
}
