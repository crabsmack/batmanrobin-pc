#include "game.h"

/* Mot_SeekWorldXOrBranch and Op2_SeekWorldXRange ($0019F2-$001A35): the world-x forms of the
 * screen-x seeks. */

/* the taken branch: script + the offset word (from the word itself), then NEXT */
static void branch(uint32_t *handler, const u16 **script)
{{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}}

/* Mot_SeekWorldXOrBranch - op tail of the world-x OrBranch seek ops: the world x vs the target point's x with a tolerance: outside it skips the offset word and cruises (Op2_CruiseRight / Op2_CruiseLeft), inside it clears the x target speed and takes the branch. The tests are the signed results of add.w / sub.w (N xor V).
 * In:  tolerance; script = word branch offset; self->x, mot->point_x
 * Out: diff = world x - point x + tolerance, then - 2 * tolerance (as far as the
 *      tests got); the cruise exit (script past the offset), or mot->target_x = 0 and the branch
 * Evidence: move.w $3E(a6),d0 / sub.w $14(a3),d0 / add.w d1,d0 / blt / sub.w d1,d0 / sub.w d1,d0 / bge; adda.w (a2),a2 on arrival, addq.l #2 then bra.w the cruise exit otherwise
 */
void Mot_SeekWorldXOrBranch(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script,
            Vm2Motion *mot, const Vm2Obj *self)
{
    int16_t d = (int16_t)(uint16_t)(self->x - mot->point_x);
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

/* Op2_SeekWorldXRange - a2 op: keeps the object's world x inside [lo, hi]: below lo -> Op2_CruiseRight, above hi -> Op2_CruiseLeft, inside -> the x target speed cleared.
 * In:  script = word lo, word hi (world pixels); self->x
 * Out: x = the world x; the cruise exit (script past both words), or mot->target_x = 0 and NEXT
 * Evidence: move.w $3E(a6),d0 / cmp.w (a2)+,d0 / blt.s / cmp.w (a2)+,d0 / bgt.w Op2_CruiseLeft / clr.l $24(a3) / NEXT; addq.l #2,a2 / bra.w Op2_CruiseRight
 */
void Op2_SeekWorldXRange(uint16_t *x, uint32_t *handler, const u16 **script, Vm2Motion *mot,
            const Vm2Obj *self)
{
    *x = (uint16_t)self->x;
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
