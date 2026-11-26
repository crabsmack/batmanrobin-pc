#include "game.h"

/* NEXT: the handler the script's next word names, jumped to */
static void op_next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the object's 16.16 position (y at +$42) */
static uint32_t pos_y(const Vm2Obj *o) { return (uint32_t)(uint16_t)o->y << 16 | o->y_frac; }
static uint32_t pos_x(const Vm2Obj *o) { return (uint32_t)(uint16_t)o->x << 16 | o->x_frac; }
static void set_pos_x(Vm2Obj *o, uint32_t v) { o->x = (int16_t)(v >> 16); o->x_frac = (uint16_t)v; }
static void set_pos_y(Vm2Obj *o, uint32_t v) { o->y = (int16_t)(v >> 16); o->y_frac = (uint16_t)v; }

/* Op2_ApplyMoveBranchAtY - a2 op: adds this frame's move to x / y; when y reaches a word limit (from
 * above or below) it is clamped to the limit and the script branches.
 * In:  script = word limit (whole pixels), word branch offset; d1 / d2 = the move (16.16)
 * Out: x += d1; y += d2, or y = limit and the branch (Op2_Goto's adda.w (a2),a2); d0 = limit << 16;
 *      not reached: Op2_SkipWord (the offset skipped, NEXT)
 * Evidence: add.l d1,$3E(a6) / moveq #0,d0 / move.w (a2)+,d0 / swap d0 / cmp.l $42(a6),d0 / bgt.s:
 *           add.l d2,$42(a6) / cmp.l $42(a6),d0 / blt.w $7E4 (or bgt.w $7E4 from above) /
 *           move.l d0,$42(a6) / adda.w (a2),a2 / NEXT; Scr2_01DEB6 at $01E044
 *           (`4B74 50E6 000E 4C5E 00B4`); never run in the corpus
 */
void Op2_ApplyMoveBranchAtY(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    set_pos_x(self, pos_x(self) + d1);
    *d0 = (uint32_t)*(*script)++ << 16;
    bool above = (int32_t)*d0 > (int32_t)pos_y(self);     /* the limit is below the object */
    set_pos_y(self, pos_y(self) + d2);
    int32_t y = (int32_t)pos_y(self);
    if (above ? (int32_t)*d0 > y : (int32_t)*d0 < y) {
        dk::jump(at_Op2_SkipWord);                       /* not reached yet */
        return;
    }
    set_pos_y(self, *d0);
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    op_next(a0, script);
}
