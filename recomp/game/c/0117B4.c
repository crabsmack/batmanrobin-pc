#include "game.h"

/* HudOp_WaitSlide - panel script op $14: waits until the panel's slide has stopped (speed +$12 = 0),
 * re-running itself every frame, then runs on.
 * In:  a0 = the next op; rec = the player record (a6)
 * Out: at rest: HUD_PanelNext; sliding: +$20 = this op, +$1E = 1, a0 back on the op word
 * Evidence: tst.l $12(a6) / beq.s HUD_PanelNext / subq.l #2,a0 / move.l a0,$20(a6) / move.w
 *           #1,$1E(a6) / rts; entry $14 of Tbl_011764; follows HudOp_SlideOut in Tbl_010F76,
 *           $10FB2, $10FD2
 */
void HudOp_WaitSlide(uint32_t *a0, uint32_t *a1, PlayerRec *rec)
{
    (void)a1;
    if (rec->panel_speed == 0) {
        dk::jump(at_HUD_PanelNext);
        return;
    }
    *a0 -= 2;
    rec->panel_script = *a0;
    rec->panel_timer = 1;
}
