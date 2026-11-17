#include "game.h"

/* The not-on-floor branch ops on the last move's floor result (a3 +$54) ($001F66-$001F81). */

/* the taken branch: script + the offset word (from the word itself), then NEXT */
static void branch(uint32_t *handler, const u16 **script)
{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_BranchNotFloorMask - a2 op: branches when the floor result has none of the mask's bits
 * (run every frame by the walkers).
 * In:  script = word mask, word branch offset; mot->contact
 * Out: hits = mask & contact; the branch when 0, else Op2_SkipWord
 * Evidence: move.w (a2)+,d0 / and.w $54(a3),d0 / bne.w Op2_SkipWord / adda.w (a2),a2 / NEXT; 89
 *           corpus runs incl. the TAS (36584 TAS frames)
 */
void Op2_BranchNotFloorMask(uint16_t *hits, uint32_t *handler, const u16 **script, const Vm2Motion *mot)
{
    *hits = *(*script)++;
    *hits = (uint16_t)(*hits & mot->contact);
    if (*hits != 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    branch(handler, script);
}

/* Op2_BranchOffFloor - a2 op: branches when the object touched no floor on its last move
 * (airborne).
 * In:  script = word branch offset; mot->contact
 * Out: the branch when contact = 0, else Op2_SkipWord
 * Evidence: tst.w $54(a3) / bne.w Op2_SkipWord / adda.w (a2),a2 / NEXT; 20 corpus runs incl. the TAS
 */
void Op2_BranchOffFloor(uint32_t *handler, const u16 **script, const Vm2Motion *mot)
{
    if (mot->contact != 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    branch(handler, script);
}
