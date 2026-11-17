#include "game.h"

/* Obj_SyncChild ($002550-$0025E3; the child-to-parent mirror at $25E4 is raw bytes, never run). */

/* the next flag bit: add.w d2,d2 / bcc (the bit shifted out into C) */
static bool take_flag(uint16_t *flags)
{
    bool set = (*flags & 0x8000u) != 0;
    *flags = (uint16_t)(*flags << 1);
    return set;
}

/* Obj_SyncChild - copies the parent's fields to the child object in an a6 field, as a flags word
 * selects, from bit 15 down: 15 x/y -> x/y, 14 drawn -> drawn, 13 x/y -> drawn, 12 drawn -> x/y,
 * 11 add a word pair to the child's x/y, 10 add a word pair to its drawn position, 9 the sprite
 * attributes and 8 the sort depth to the child's sprite block. With no child only the word pairs
 * are skipped.
 * In:  script = word a6 field offset, word flags, then a word pair for each of bits 11/10;
 *      self / spr = the parent and its sprite block
 * Out: field (d0), child (d1: its handle, 0 = none), flags (d2) shifted past the bits read, pair
 *      (d3) = the last pair word read; script past the operands
 * Evidence: move.w (a2)+,d0 / move.w (a2)+,d2 / move.w (a6,d0.w),d1 / bne / lsl.w #5,d2 / bcc /
 *           addq.l #4,a2 ..., then the add.w d2,d2 / bcc chain; ops $2506/$2548 call it; 34 runs
 */
void Obj_SyncChild(uint16_t *field, uint16_t *child, uint16_t *flags, uint16_t *pair, const u16 **script,
                   const Vm2Sprite *spr, const Vm2Obj *self)
{
    *field = *(*script)++;
    *flags = *(*script)++;
    *child = *(const u16 *)((const u8 *)self + (int16_t)*field);
    if (*child == 0) {
        bool add_pos = (*flags >> 11) & 1;   /* lsl.w #5: the last bit out is bit 11 */
        *flags = (uint16_t)(*flags << 5);
        if (add_pos)
            *script += 2;
        if (take_flag(flags))
            *script += 2;
        return;
    }
    Vm2Obj *kid = dk::handle<Vm2Obj>((int16_t)*child);
    if (take_flag(flags)) {
        kid->x = self->x;
        kid->y = self->y;
    }
    if (take_flag(flags)) {
        kid->draw_x = self->draw_x;
        kid->draw_y = self->draw_y;
    }
    if (take_flag(flags)) {
        kid->draw_x = self->x;
        kid->draw_y = self->y;
    }
    if (take_flag(flags)) {
        kid->x = self->draw_x;
        kid->y = self->draw_y;
    }
    if (take_flag(flags)) {
        *pair = *(*script)++;
        kid->x = (int16_t)(kid->x + (int16_t)*pair);
        *pair = *(*script)++;
        kid->y = (int16_t)(kid->y + (int16_t)*pair);
    }
    if (take_flag(flags)) {
        *pair = *(*script)++;
        kid->draw_x = (int16_t)(kid->draw_x + (int16_t)*pair);
        *pair = *(*script)++;
        kid->draw_y = (int16_t)(kid->draw_y + (int16_t)*pair);
    }
    Vm2Sprite *kid_spr = dk::handle<Vm2Sprite>(kid->sprite);
    if (take_flag(flags))
        kid_spr->attr = spr->attr;
    if (take_flag(flags))
        kid_spr->depth = spr->depth;
}
