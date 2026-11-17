#include "game.h"

/* Op2_BranchWallMask - a2 op: branches when the last move's wall contact shares a bit with the
 * mask word.
 * In:  script = word mask, word branch offset; mot->wall_hit (Coll_MoveScene's d6, stored by
 *      the move op $4CDA)
 * Out: hits = mask & wall_hit; the branch (script + offset, NEXT), else Op2_SkipWord
 * Evidence: move.w (a2)+,d0 / and.w $52(a3),d0 / beq.w Op2_SkipWord / adda.w (a2),a2 / NEXT; 16
 *           corpus runs incl. the TAS
 */
void Op2_BranchWallMask(uint16_t *hits, uint32_t *handler, const u16 **script, const Vm2Motion *mot)
{
    *hits = *(*script)++;
    *hits = (uint16_t)(*hits & mot->wall_hit);
    if (*hits == 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
