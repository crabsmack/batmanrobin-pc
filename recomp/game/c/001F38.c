#include "game.h"

/* Op2_BranchNoWall - a2 op: branches when the last move hit no wall.
 * In:  script = word branch offset; mot->wall_hit
 * Out: the branch (script + offset, NEXT) when wall_hit = 0, else Op2_SkipWord
 * Evidence: tst.w $52(a3) / bne.w Op2_SkipWord / adda.w (a2),a2 / NEXT; 8 corpus runs incl. the TAS
 */
void Op2_BranchNoWall(uint32_t *handler, const u16 **script, const Vm2Motion *mot)
{
    if (mot->wall_hit != 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
