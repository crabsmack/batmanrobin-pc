#include "game.h"

/* Mot_SetTarget - stores a target point in the motion block and classifies the direction to it:
 * MOT_VERTICAL when |dy| >= |dx|, MOT_NEGATIVE when that dominant component is negative.
 * In:  x / y = the target point; self's position
 * Out: x / y = dx / dy (target - position); ady = |dy| (|dx| is computed too, but no caller reads
 *      d2); mot->point_x / point_y, mot->flags bits 3/4
 * Evidence: move.w d0,$14(a3) / move.w d1,$16(a3) / sub.w $3E(a6),d0 / neg / sub.w $42(a6),d1 /
 *           cmp.w d2,d3 / blt / bset #3,(a3) / tst.w d1 / bmi / bset or bclr #4,(a3); callers the
 *           aim ops $1364-$13A4; Op2_BranchPastTarget reads the bits; 765 TAS frames
 */
void Mot_SetTarget(uint16_t *x, uint16_t *y, uint16_t *ady, Vm2Motion *mot, const Vm2Obj *self)
{
    mot->point_x = (int16_t)*x;
    mot->point_y = (int16_t)*y;
    int16_t dx = (int16_t)(*x - self->x);
    int16_t dy = (int16_t)(*y - self->y);
    *x = (uint16_t)dx;
    *y = (uint16_t)dy;
    int16_t adx = (int16_t)(dx < 0 ? -dx : dx);   /* neg.w: -$8000 stays -$8000 */
    *ady = (uint16_t)(dy < 0 ? -dy : dy);
    bool negative;
    if ((int16_t)*ady < adx) {
        mot->flags &= (u8)~MOT_VERTICAL;
        negative = dx < 0;
    } else {
        mot->flags |= MOT_VERTICAL;
        negative = dy < 0;
    }
    if (negative)
        mot->flags |= MOT_NEGATIVE;
    else
        mot->flags &= (u8)~MOT_NEGATIVE;
}
