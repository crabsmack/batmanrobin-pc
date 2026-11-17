#include "game.h"

/* Hitbox_OverlapCenter - the centre of the intersection of box's rectangle and the rectangle
 * x1..x2 x y1..y2: the larger of the left/top edges, the smaller of the right/bottom ones, averaged.
 * The edge tests are `cmp` + `bmi` (the sign of the 16-bit difference, not a signed compare), kept.
 * In:  box (a2); x1/y1/x2/y2 = the other rectangle (d4/d5/d6/d7 low words)
 * Out: x1/y1 = the centre (d4/d5); x2/y2 = the intersection's right/bottom (d6/d7); d0/d1 = the
 *      box's left/top sign-extended (movem.w)
 * Evidence: movem.w $12(a2),d0-d3 / cmp.w d4,d0 / bmi / move.w d0,d4 (likewise d2/d6, d1/d5,
 *           d3/d7) / add.w d6,d4 / asr.w #1,d4 / add.w d7,d5 / asr.w #1,d5; only caller
 *           Hitbox_SpawnAtContact
 */
void Hitbox_OverlapCenter(uint32_t *d0, uint32_t *d1, uint16_t *x1, uint16_t *y1, uint16_t *x2, uint16_t *y2,
                          const HitBox *box)
{
    int16_t left = box->left, top = box->top, right = box->right, bottom = box->bottom;
    *d0 = (uint32_t)(int32_t)left;
    *d1 = (uint32_t)(int32_t)top;
    if ((int16_t)(left - *x1) >= 0)
        *x1 = (uint16_t)left;
    if ((int16_t)(*x2 - right) >= 0)
        *x2 = (uint16_t)right;
    if ((int16_t)(top - *y1) >= 0)
        *y1 = (uint16_t)top;
    if ((int16_t)(*y2 - bottom) >= 0)
        *y2 = (uint16_t)bottom;
    *x1 = (uint16_t)((int16_t)(uint16_t)(*x1 + *x2) >> 1);
    *y1 = (uint16_t)((int16_t)(uint16_t)(*y1 + *y2) >> 1);
}
