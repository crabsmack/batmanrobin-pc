#include "game.h"

/* Op2_BranchNotSection / Op2_SetShakeTimer ($0042FE-$004325). */

/* Op2_BranchNotSection - a2 op: branches (Op2_Goto) unless the current game section is the
 * operand.
 * In:  script = word section id, word offset; g_SectionId
 * Out: section = the operand; the branch with the script on the offset, else handler = NEXT's
 *      after the offset
 * Evidence: move.w (a2)+,d0 / cmp.w (g_SectionId).w,d0 / bne.w Op2_Goto / addq.l #2,a2 / NEXT;
 *           15 corpus runs; $1B8D4 `42FE 1100 000C` (stage 1 section $1100)
 */
void Op2_BranchNotSection(uint16_t *section, uint32_t *handler, const u16 **script)
{
    *section = *(*script)++;
    if (*section != g_SectionId) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SetShakeTimer - a2 op: sets the sprite's shake timer: while it runs Op2_Draw draws the
 * sprite displaced by a random -2..1 px.
 * In:  script = word frames (the low byte used)
 * Out: frames (d0) = the word; spr->shake; NEXT
 * Evidence: move.w (a2)+,d0 / move.b d0,$17(a5) / NEXT; Op2_Draw counts $17(a5) down and adds
 *           Rand_Next >> 14 to x / y; 74 corpus runs; $19844 `430E 001E`
 */
void Op2_SetShakeTimer(uint16_t *frames, const u16 **script, Vm2Sprite *spr)
{
    *frames = *(*script)++;
    spr->shake = (uint8_t)*frames;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
