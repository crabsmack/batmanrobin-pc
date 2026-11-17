#include "game.h"

/* Mot_SeekWorldYOrBranch and Op2_SeekWorldYRange ($001B32-$001B75): the world-y forms. */

/* the taken branch: script + the offset word (from the word itself), then NEXT */
static void branch(uint32_t *handler, const u16 **script)
{{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}}

/* Mot_SeekWorldYOrBranch - op tail of the world-y OrBranch seek ops: the world y vs the target point's y with a tolerance: outside it skips the offset word and cruises (Op2_CruiseDown / Op2_CruiseUp), inside it clears the y target speed and takes the branch. The tests are the signed results of add.w / sub.w (N xor V).
 * In:  tolerance; script = word branch offset; self->y, mot->point_y
 * Out: diff = world y - point y + tolerance, then - 2 * tolerance (as far as the
 *      tests got); the cruise exit (script past the offset), or mot->target_y = 0 and the branch
 * Evidence: move.w $42(a6),d0 / sub.w $16(a3),d0 / add.w d1,d0 / blt / sub.w d1,d0 / sub.w d1,d0 / bge; adda.w (a2),a2 on arrival, addq.l #2 then bra.w the cruise exit otherwise
 */
void Mot_SeekWorldYOrBranch(uint16_t *diff, uint16_t tolerance, uint32_t *handler, const u16 **script,
            Vm2Motion *mot, const Vm2Obj *self)
{
    int16_t d = (int16_t)(uint16_t)(self->y - mot->point_y);
    int32_t sum = d + (int16_t)tolerance;
    *diff = (uint16_t)sum;
    if (sum < 0) {
        (*script)++;
        dk::jump(at_Op2_CruiseDown);
        return;
    }
    *diff = (uint16_t)(*diff - tolerance);
    int32_t rest = (int16_t)*diff - (int16_t)tolerance;
    *diff = (uint16_t)rest;
    if (rest >= 0) {
        (*script)++;
        dk::jump(at_Op2_CruiseUp);
        return;
    }
    mot->target_y = 0;
    branch(handler, script);
}

/* Op2_SeekWorldYRange - a2 op: keeps the object's world y inside [lo, hi]: below lo -> Op2_CruiseDown, above hi -> Op2_CruiseUp, inside -> the y target speed cleared.
 * In:  script = word lo, word hi (world pixels); self->y
 * Out: y = the world y; the cruise exit (script past both words), or mot->target_y = 0 and NEXT
 * Evidence: move.w $42(a6),d0 / cmp.w (a2)+,d0 / blt.s / cmp.w (a2)+,d0 / bgt.w Op2_CruiseUp / clr.l $2C(a3) / NEXT; addq.l #2,a2 / bra.w Op2_CruiseDown
 */
void Op2_SeekWorldYRange(uint16_t *y, uint32_t *handler, const u16 **script, Vm2Motion *mot,
            const Vm2Obj *self)
{
    *y = (uint16_t)self->y;
    if ((int16_t)*y < (int16_t)*(*script)++) {
        (*script)++;
        dk::jump(at_Op2_CruiseDown);
        return;
    }
    if ((int16_t)*y > (int16_t)*(*script)++) {
        dk::jump(at_Op2_CruiseUp);
        return;
    }
    mot->target_y = 0;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
