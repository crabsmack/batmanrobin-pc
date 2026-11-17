#include "game.h"

/* Mot_SeekScreenX and the entries of its branching form (Op2_SeekScreenX{,8}OrBranch)
 * ($001922-$00194D). */

/* Mot_SeekScreenX - op tail of Op2_SeekScreenX/X4: compares the object's screen x with the
 * target point's x: more than the tolerance left of it -> Op2_CruiseRight, the tolerance or more
 * right of it -> Op2_CruiseLeft, else the x target speed is cleared. The two tests are the
 * signed results of add.w / sub.w (N xor V), so a word overflow tests the true sum.
 * In:  tolerance; self->x, g_CameraX, mot->point_x
 * Out: diff = screen x - point x + tolerance, then - 2 * tolerance (as far as the tests got);
 *      the exit jumped to, or mot->target_x = 0 and NEXT
 * Evidence: move.w $3E(a6),d0 / sub.w (g_CameraX).w,d0 / sub.w $14(a3),d0 / add.w d1,d0 /
 *           blt.w Op2_CruiseRight / sub.w d1,d0 / sub.w d1,d0 / bge.w Op2_CruiseLeft /
 *           clr.l $24(a3) / NEXT; executed in 11 runs incl. the TAS (frames 77185-130448)
 */
void Mot_SeekScreenX(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script,
                     Vm2Motion *mot, const Vm2Obj *self)
{
    int16_t d = (int16_t)(uint16_t)(self->x - (uint16_t)(g_CameraX >> 16) - mot->point_x);
    int32_t sum = d + (int16_t)tolerance;
    *diff = (uint16_t)sum;
    if (sum < 0) {
        dk::jump(at_Op2_CruiseRight);
        return;
    }
    *diff = (uint16_t)(*diff - tolerance);
    int32_t rest = (int16_t)*diff - (int16_t)tolerance;
    *diff = (uint16_t)rest;
    if (rest >= 0) {
        dk::jump(at_Op2_CruiseLeft);
        return;
    }
    mot->target_x = 0;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SeekScreenXOrBranch - a2 op: steers toward the target point's screen x like
 * Op2_SeekScreenX, and branches once within the given tolerance.
 * In:  script = word tolerance, word branch offset
 * Out: tolerance = the word; then Mot_SeekScreenXOrBranch (jumped to)
 * Evidence: move.w (a2)+,d1 / bra.s Mot_SeekScreenXOrBranch; 12 corpus runs incl. the TAS
 */
void Op2_SeekScreenXOrBranch(uint16_t *d0, uint16_t *tolerance, uint32_t *handler, const u16 **script,
                             const Vm2Motion *mot, const Vm2Obj *self)
{
    *tolerance = *(*script)++;
    dk::jump(at_Mot_SeekScreenXOrBranch);
}

/* Op2_SeekScreenX8OrBranch - a2 op: Mot_SeekScreenXOrBranch with an 8-pixel tolerance.
 * In:  script = word branch offset
 * Out: tolerance = 8; then Mot_SeekScreenXOrBranch (jumped to)
 * Evidence: moveq #8,d1 / bra.s Mot_SeekScreenXOrBranch; 14 corpus runs incl. the TAS
 */
void Op2_SeekScreenX8OrBranch(uint16_t *d0, uint32_t *tolerance, uint32_t *handler, const u16 **script,
                              const Vm2Motion *mot, const Vm2Obj *self)
{
    *tolerance = 8;
    dk::jump(at_Mot_SeekScreenXOrBranch);
}
