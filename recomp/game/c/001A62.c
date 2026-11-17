#include "game.h"

/* Mot_SeekScreenY and the entries of its branching form (Op2_SeekScreenY{,8}OrBranch)
 * ($001A62-$001A8D). */

static int16_t screen_y(const Vm2Obj *self) { return (int16_t)(self->y - (int16_t)(g_CameraY >> 16)); }

/* Mot_SeekScreenY - op tail of Op2_SeekScreenY4: the screen y vs the target point's y: more than the tolerance above it -> Op2_CruiseDown, the tolerance or more below it -> Op2_CruiseUp, else the y target speed is cleared. The tests are the signed results of add.w / sub.w (N xor V).
 * In:  tolerance; self->y, g_CameraY, mot->point_y
 * Out: diff = screen y - point y + tolerance, then - 2 * tolerance (as far as the
 *      tests got); the exit jumped to, or mot->target_y = 0 and NEXT
 * Evidence: move.w $42(a6),d0 / sub.w (g_CameraY).w,d0 / sub.w $16(a3),d0 / add.w d1,d0 / blt / sub.w d1,d0 / sub.w d1,d0 / bge; clr.l $2C(a3) / NEXT inside
 */
void Mot_SeekScreenY(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script,
            Vm2Motion *mot, const Vm2Obj *self)
{
    int16_t d = (int16_t)(uint16_t)(screen_y(self) - mot->point_y);
    int32_t sum = d + (int16_t)tolerance;
    *diff = (uint16_t)sum;
    if (sum < 0) {
        dk::jump(at_Op2_CruiseDown);
        return;
    }
    *diff = (uint16_t)(*diff - tolerance);
    int32_t rest = (int16_t)*diff - (int16_t)tolerance;
    *diff = (uint16_t)rest;
    if (rest >= 0) {
        dk::jump(at_Op2_CruiseUp);
        return;
    }
    mot->target_y = 0;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SeekScreenYOrBranch - a2 op: steers toward the target point's screen y like Op2_SeekScreenY4, and branches once within the given tolerance.
 * In:  script = word tolerance, word branch offset
 * Out: tolerance = the word; then Mot_SeekScreenYOrBranch (jumped to)
 * Evidence: move.w (a2)+,d1 / bra.s Mot_SeekScreenYOrBranch
 */
void Op2_SeekScreenYOrBranch(uint16_t *d0, uint16_t *tolerance, uint32_t *handler, const u16 **script,
            const Vm2Motion *mot, const Vm2Obj *self)
{
    *tolerance = *(*script)++;
    dk::jump(at_Mot_SeekScreenYOrBranch);
}

/* Op2_SeekScreenY8OrBranch - a2 op: Mot_SeekScreenYOrBranch with an 8-pixel tolerance.
 * In:  script = word branch offset
 * Out: tolerance = 8; then Mot_SeekScreenYOrBranch (jumped to)
 * Evidence: moveq #8,d1 / bra.s Mot_SeekScreenYOrBranch
 */
void Op2_SeekScreenY8OrBranch(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script,
            const Vm2Motion *mot, const Vm2Obj *self)
{
    *tolerance = 8;
    dk::jump(at_Mot_SeekScreenYOrBranch);
}
