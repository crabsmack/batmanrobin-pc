#include "game.h"

/* Op4_SaveDrawPos / Op4_ChildSaveDrawPos ($0057C0-$00582B): the drawn point (x, y) + radius *
 * (cos, sin)(heading), its pixel words stored for the terrain tests and the next spawns. */

/* Tbl_059B22: the sine table, a signed word per even heading (index & $3FE as a byte offset);
   the cosine a quarter turn on (+$100 bytes) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;
static const s16 *sine_entry(uint16_t byte_index)
{
    PTR(const s16) e;
    e.v = Tbl_059B22 + byte_index;
    return e;
}

/* The drawn point of `obj`, turned by `heading_of`'s heading: d6/d7 = x/y (16.16) plus radius
 * times (cos, sin) (muls.w on the radius word; nothing added for radius 0), then swapped so the
 * pixel words are low; stored in obj's draw_x/draw_y. d0/d1 keep the products (d0 the radius word
 * when the radius is 0). */
static void save_draw_pos(uint32_t *d0, uint32_t *d1, uint32_t *d6, uint32_t *d7, const Vm4Obj *heading_of,
                          Vm4Obj *obj)
{
    *d6 = vm4_fix(obj->x, obj->x_frac);
    *d7 = vm4_fix(obj->y, obj->y_frac);
    uint16_t radius = (uint16_t)obj->radius;
    *d0 = (*d0 & 0xFFFF0000u) | radius;
    if (radius != 0) {
        const s16 *sine = sine_entry((uint16_t)heading_of->heading & 0x3FE);
        *d0 =(uint32_t)((int32_t)(int16_t)radius * sine[0x80]);
        *d1 = (uint32_t)((int32_t)(int16_t)radius * sine[0]);
        *d6 += *d0;
        *d7 += *d1;
    }
    *d6 = *d6 << 16 | *d6 >> 16;
    *d7 = *d7 << 16 | *d7 >> 16;
    obj->draw_x = (int16_t)(uint16_t)*d6;
    obj->draw_y = (int16_t)(uint16_t)*d7;
}

/* Op4_SaveDrawPos - a4 op: computes the object's drawn point and stores its pixel words. The
 * heading is read from a5 (the last spawned child, the object itself until a spawn), as in the
 * original.
 * In:  self = a6; child = a5 (its heading turns the radius)
 * Out: d6/d7 = the drawn x/y, swapped (pixel words low); d0/d1 the radius products; self->draw_x,
 *      draw_y; NEXT
 * Evidence: movem.l $2E(a6),d6-d7 / move.w $26(a6),d0 / beq / lea Tbl_059B22,a0 / move.w $2A(a5),d1
 *           / andi.w #$3FE,d1 / muls.w $100(a0),d0 / muls.w (a0),d1 / add.l / swap d6 / swap d7 /
 *           movem.w d6-d7,$56(a6); script uses $1F200, $26BE6, $26D4C; 6 corpus runs
 */
void Op4_SaveDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d6, uint32_t *d7, const u16 **script,
                     const Vm4Obj *child, Vm4Obj *self)
{
    save_draw_pos(d0, d1, d6, d7, child, self);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op4_ChildSaveDrawPos - a4 op: computes the last spawned child's drawn point and stores it in the
 * child.
 * In:  child = a5
 * Out: as Op4_SaveDrawPos on the child; the condition codes of the swap of d7; NEXT
 * Evidence: the Op4_SaveDrawPos code on a5 throughout; 38 a4-script uses, e.g. $1A394, $1A640
 *           (after the child's velocity ops, before the next spawn); 3 corpus runs
 */
void Op4_ChildSaveDrawPos(uint32_t *d0, uint32_t *d1, uint32_t *d6, uint32_t *d7, uint32_t *handler,
                          const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    save_draw_pos(d0, d1, d6, d7, child, child);
    *ccr = dk::Flags::test(*d7, 4, ccr->x);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
