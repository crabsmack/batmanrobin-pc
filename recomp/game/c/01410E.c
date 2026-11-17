#include "game.h"

/* Text_DrawAtObject - a2 native (op $2CBC): draws the string named by the dc.l after the op as
 * sprite letters at the object's position, in its sprite block's attributes. a1-a6 are kept
 * across the drawing (saved on the 68000's stack, as the original's movem).
 * In:  script = at the dc.l (the string's address); sprite = the object's sprite block; self = the
 *      object
 * Out: script past the long; x/y/attr = the object's x, y and attr, x advanced past the string;
 *      d0-d3 and the condition codes as Text_DrawSprites leaves them
 * Evidence: move.w $3E(a6),d4 / move.w $42(a6),d5 / movea.l (a2)+,a0 / movem.l a1-a6,-(a7) / movea.l
 *           a0,a2 / move.w $38(a5),d6 / jsr Text_DrawSprites / movem.l (a7)+,a1-a6; op $2CBC pointers
 *           at $49668 and $4A29C-$4A51E (the stage cards) to the strings $1407A-$1410D
 */
void Text_DrawAtObject(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *x, uint16_t *y,
                       uint16_t *attr, const PTR(const u8) **script, const Vm2Sprite *sprite, const Vm2Obj *self,
                       dk::Flags *ccr)
{
    *x = (uint16_t)self->x;
    *y = (uint16_t)self->y;
    const u8 *text = **script;
    (*script)++;
    /* movem.l a1-a6,-(a7): a1, a3 and a4 are not inputs (nothing reads them back) */
    dk::push_long(dk::addr(self));
    dk::push_long(dk::addr(sprite));
    dk::push_long(0);
    dk::push_long(0);
    dk::push_long(dk::addr(*script));
    dk::push_long(0);
    *attr = sprite->attr;
    Text_DrawSprites(d0, d1, d2, d3, x, *y, *attr, text, ccr);
    for (int i = 0; i < 6; i++)
        dk::pop_long();                      /* movem.l (a7)+,a1-a6: the entry values stand */
}
