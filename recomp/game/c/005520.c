#include "game.h"

/* Op4_BranchScreenYOutside - a4 op: branches when the drawn y's screen row (d5 minus the camera
 * y) is outside [lo, hi]; inside, continues past the three operands. Below lo the branch is taken
 * with the script at the hi word (the cmp's postincrement), so its target is hi's address plus
 * hi: the original's quirk, kept.
 * In:  draw_y = d5 (the drawn y word after a motion op); script = word lo, word hi, word offset
 * Out: d0 = draw_y - camera y; Op4_Goto when outside (from the hi word or the offset word), else
 *      NEXT after the operands
 * Evidence: move.w d5,d0 / sub.w (g_CameraY).w,d0 / cmp.w (a4)+,d0 / blt Op4_Goto / cmp.w (a4)+,d0 /
 *           bgt Op4_Goto / addq.l #2,a4 / NEXT; uses at $3CE6C, $3CED2 (lo -$64, hi $94, offset $A)
 *           after the move op $632E; 3 corpus runs, 2456 TAS frames
 */
void Op4_BranchScreenYOutside(uint16_t *d0, uint16_t draw_y, uint32_t *handler, const u16 **script)
{
    *d0 = (uint16_t)(draw_y - (uint16_t)(g_CameraY >> 16));
    int16_t row = (int16_t)*d0;
    if (row < (int16_t)(uint16_t)*(*script)++) {
        dk::jump(at_Op4_Goto);
        return;
    }
    if (row > (int16_t)(uint16_t)*(*script)++) {
        dk::jump(at_Op4_Goto);
        return;
    }
    (*script)++;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}
