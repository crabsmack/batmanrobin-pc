#include "game.h"

/* Mot_SeekScreenXOrBranch and the screen-x range ops ($00194E-$0019C1). Each steers through the
 * cruise exits Op2_CruiseRight / Op2_CruiseLeft (jumped to with the script past the operands), or
 * clears the x target speed when there. */

static int16_t screen_x(const Vm2Obj *self) { return (int16_t)(self->x - (int16_t)(g_CameraX >> 16)); }

/* the taken branch: script + the offset word (from the word itself), then NEXT */
static void branch(uint32_t *handler, const u16 **script)
{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Mot_SeekScreenXOrBranch - op tail of Op2_SeekScreenX(8)OrBranch: the screen x vs the target
 * point's x with a tolerance: outside it skips the offset word and cruises right / left, inside
 * it clears the x target speed and takes the branch. The tests are the signed results of add.w /
 * sub.w (N xor V).
 * In:  tolerance; script = word branch offset; self->x, g_CameraX, mot->point_x
 * Out: diff as Mot_SeekScreenX leaves it; the cruise exit (script past the offset), or
 *      mot->target_x = 0 and the branch
 * Evidence: the compare of Mot_SeekScreenX; blt.s / bge.s to addq.l #2,a2 + bra.w Op2_CruiseRight /
 *           Op2_CruiseLeft; clr.l $24(a3) / adda.w (a2),a2 / NEXT; 25 runs incl. the TAS
 */
void Mot_SeekScreenXOrBranch(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script,
                             Vm2Motion *mot, const Vm2Obj *self)
{
    int16_t d = (int16_t)(uint16_t)(screen_x(self) - mot->point_x);
    int32_t sum = d + (int16_t)tolerance;
    *diff = (uint16_t)sum;
    if (sum < 0) {
        (*script)++;
        dk::jump(at_Op2_CruiseRight);
        return;
    }
    *diff = (uint16_t)(*diff - tolerance);
    int32_t rest = (int16_t)*diff - (int16_t)tolerance;
    *diff = (uint16_t)rest;
    if (rest >= 0) {
        (*script)++;
        dk::jump(at_Op2_CruiseLeft);
        return;
    }
    mot->target_x = 0;
    branch(handler, script);
}

/* Op2_SeekScreenXRange - a2 op: keeps the object's screen x inside [lo, hi]: left of lo ->
 * Op2_CruiseRight, right of hi -> Op2_CruiseLeft, inside -> the x target speed cleared.
 * In:  script = word lo, word hi (screen pixels); self->x, g_CameraX
 * Out: x = the screen x; the cruise exit (script past both words), or mot->target_x = 0 and NEXT
 * Evidence: move.w $3E(a6),d0 / sub.w (g_CameraX).w,d0 / cmp.w (a2)+,d0 / blt.s / cmp.w (a2)+,d0 /
 *           bgt.w Op2_CruiseLeft / clr.l $24(a3) / NEXT; addq.l #2,a2 / bra.w Op2_CruiseRight; 7
 *           corpus runs incl. the TAS
 */
void Op2_SeekScreenXRange(uint16_t *x, uint32_t *handler, const u16 **script, Vm2Motion *mot,
                          const Vm2Obj *self)
{
    *x = (uint16_t)screen_x(self);
    if ((int16_t)*x < (int16_t)*(*script)++) {
        (*script)++;
        dk::jump(at_Op2_CruiseRight);
        return;
    }
    if ((int16_t)*x > (int16_t)*(*script)++) {
        dk::jump(at_Op2_CruiseLeft);
        return;
    }
    mot->target_x = 0;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SeekScreenXRangeOrBranch - a2 op: as Op2_SeekScreenXRange, and branches when the screen x
 * is already inside [lo, hi].
 * In:  script = word lo, word hi, word branch offset; self->x, g_CameraX
 * Out: x = the screen x; the cruise exit (script past all three words), or mot->target_x = 0 and
 *      the branch
 * Evidence: cmp.w (a2)+ twice; inside clr.l $24(a3) / adda.w (a2),a2 / NEXT; addq.l #2 /
 *           bra.w Op2_CruiseLeft above hi, addq.l #4 / bra.w Op2_CruiseRight below lo; 14 corpus
 *           runs incl. the TAS
 */
void Op2_SeekScreenXRangeOrBranch(uint16_t *x, uint32_t *handler, const u16 **script, Vm2Motion *mot,
                                  const Vm2Obj *self)
{
    *x = (uint16_t)screen_x(self);
    if ((int16_t)*x < (int16_t)*(*script)++) {
        *script += 2;
        dk::jump(at_Op2_CruiseRight);
        return;
    }
    if ((int16_t)*x > (int16_t)*(*script)++) {
        (*script)++;
        dk::jump(at_Op2_CruiseLeft);
        return;
    }
    mot->target_x = 0;
    branch(handler, script);
}
