#include "game.h"

/* HudOp_Goto - panel script op 8: jumps by the operand word (relative to the op word) and runs on.
 * In:  a0 = the operand (word offset from the op)
 * Out: a0 = the target op; then HUD_PanelNext
 * Evidence: adda.w (a0),a0 / lea -2(a0),a0 / bra.s HUD_PanelNext; entry 8 of Tbl_011764; closes the
 *           looping scripts $10F8A, $10FB2, $11012
 */
void HudOp_Goto(uint32_t *a0, uint32_t *a1)
{
    (void)a1;
    PTR(const s16) offset;
    offset.v = *a0;
    *a0 += (uint32_t)(int32_t)*offset.get() - 2;
    dk::jump(at_HUD_PanelNext);
}
