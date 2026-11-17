#include "game.h"

/* The floor-contact branch ops on the last move's floor result (a3 +$54, Coll_MoveScene's d7
 * stored by the move op $4CDA) ($001EEA-$001F05). */

/* the taken branch: script + the offset word (from the word itself), then NEXT */
static void branch(uint32_t *handler, const u16 **script)
{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_BranchFloorMask - a2 op: branches when the floor result shares a bit with the mask word
 * (landed on that kind of floor, e.g. 1 street / 2 or 4 walkway in Coll_Stage1StreetFloors).
 * In:  script = word mask, word branch offset; mot->contact
 * Out: hits = mask & contact; the branch, else Op2_SkipWord
 * Evidence: move.w (a2)+,d0 / and.w $54(a3),d0 / beq.w Op2_SkipWord / adda.w (a2),a2 / NEXT; 94
 *           corpus runs incl. the TAS
 */
void Op2_BranchFloorMask(uint16_t *hits, uint32_t *handler, const u16 **script, const Vm2Motion *mot)
{
    *hits = *(*script)++;
    *hits = (uint16_t)(*hits & mot->contact);
    if (*hits == 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    branch(handler, script);
}

/* Op2_BranchOnFloor - a2 op: branches when the object touched any floor on its last move.
 * In:  script = word branch offset; mot->contact
 * Out: the branch, else Op2_SkipWord
 * Evidence: tst.w $54(a3) / beq.w Op2_SkipWord / adda.w (a2),a2 / NEXT; 14 corpus runs incl. the TAS
 */
void Op2_BranchOnFloor(uint32_t *handler, const u16 **script, const Vm2Motion *mot)
{
    if (mot->contact == 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    branch(handler, script);
}
