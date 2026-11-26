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

/* Op2_ApplyMoveLandAtY - a2 op: adds this frame's move to x / y and lands on a floor at a word y:
 * there y is clamped to the floor, the motion block's contact word set to 1 and its y velocity
 * cleared; anywhere else the contact word is cleared.
 * In:  script = word floor y (whole pixels); d1 / d2 = the move (16.16); mot = the motion block
 * Out: x += d1; y += d2 (or the floor); mot->contact, mot->vel_y; d0 = floor << 16; NEXT
 * Evidence: add.l d1,$3E(a6) / moveq #0,d0 / move.w (a2)+,d0 / swap d0 / cmp.l $42(a6),d0 / bge.s:
 *           add.l d2,$42(a6) / cmp.l / ble.s land, else clr.w $54(a3); below: add.l d2 / cmp.l /
 *           blt.w off; land: move.l d0,$42(a6) / move.w #1,$54(a3) / clr.l $1C(a3); the op after
 *           Op2_ApplyMoveBranchAtY; never run in the corpus
 */
void Op2_ApplyMoveLandAtY(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t *a0, const u16 **script, Vm2Motion *mot,
                          Vm2Obj *self)
{
    set_pos_x(self, pos_x(self) + d1);
    *d0 = (uint32_t)*(*script)++ << 16;
    bool above = (int32_t)*d0 >= (int32_t)pos_y(self);    /* bge.s $4CC8: on or above the floor */
    set_pos_y(self, pos_y(self) + d2);
    int32_t y = (int32_t)pos_y(self);
    bool land = above ? (int32_t)*d0 <= y : (int32_t)*d0 >= y;
    if (land) {
        set_pos_y(self, *d0);
        mot->contact = 1;
        mot->vel_y = 0;
    } else {
        mot->contact = 0;
    }
    op_next(a0, script);
}
