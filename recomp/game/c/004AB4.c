#include "game.h"

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Op2_DrawSprAtDrawnBack - draw-list op (see 004454.c): one frame at the object's drawn position
 * behind every sorted sprite (Spr_AddBack).
 * In:  script = word frame; a1 = the frame slot; spr, self->draw_x / draw_y
 * Out: d4 / d5 = the drawn position, d6 = attr, d1 = the frame; the slot set, a1 + 2;
 *      Spr_ChangeFrame / Spr_AddBack's registers; handler = NEXT's
 * Evidence: move.w $38(a5),d6 / move.w $4E(a6),d4 / move.w $50(a6),d5 / move.w (a2)+,d1 /
 *           move.w (a1),d2 / bsr.w Spr_ChangeFrame / move.w d1,(a1)+ / bsr.w Spr_AddBack / NEXT;
 *           1 run; $3D544 `4AB4 4754 4AB4 475E 44F2`
 */
void Op2_DrawSprAtDrawnBack(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                            uint16_t *d6, uint32_t *a0, uint32_t *a1, const u16 **script, const Vm2Sprite *spr,
                            const Vm2Obj *self)
{
    *d6 = spr->attr;
    *d4 = (uint16_t)self->draw_x;
    *d5 = (uint16_t)self->draw_y;
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
    Spr_AddBack(&w, d1, *d4, *d5, *d6, *a0, *a1, &spr_ccr);
    set_low_word(d0, w);
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}
