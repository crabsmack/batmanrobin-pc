#include "game.h"

/* Op2_BranchPastTarget - a2 op: branches when the object has reached or passed its target point
 * along the dominant axis (Mot_SetTarget's bits): the point minus the position, on that axis, has
 * the sign of the travel direction's opposite (>= 0 moving negative, < 0 moving positive). The
 * branch also re-derives the headings from the velocity (Op2_HeadingFromVel).
 * In:  script = a word offset; mot->flags, point; self's position
 * Out: d0's low word = point - position on the axis; passed: the branch, then Op2_HeadingFromVel;
 *      else NEXT after the offset
 * Evidence: btst #3,(a3) / sub.w $3E(a6) or $42(a6) / bpl / btst #4,(a3) / adda.w (a2),a2 /
 *           bra.w Op2_HeadingFromVel; 6.1k TAS frames
 */
void Op2_BranchPastTarget(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *handler, const u16 **script,
                          Vm2Motion *mot, const Vm2Obj *self)
{
    int16_t diff = (mot->flags & MOT_VERTICAL) ? (int16_t)(mot->point_y - self->y)
                                               : (int16_t)(mot->point_x - self->x);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)diff;
    bool negative = (mot->flags & MOT_NEGATIVE) != 0;
    if ((diff >= 0) == negative) {
        *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
        dk::jump(at_Op2_HeadingFromVel);
        return;
    }
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
