#include "game.h"

/* HudOp_SlideOut - panel script op $C: starts the panel sliding off its screen edge and runs on.
 * In:  a0 = the next op; rec = the player record (a6)
 * Out: +$12/+$16 the slide, +$25 bit 6 set (HUD_StartSlideOut); then HUD_PanelNext
 * Evidence: bsr.w HUD_StartSlideOut / bra.s HUD_PanelNext; entry $C of Tbl_011764; before the
 *           content switches in Tbl_010F76 and $10FB2
 */
void HudOp_SlideOut(uint32_t *a0, uint32_t *a1, PlayerRec *rec)
{
    (void)a0, (void)a1;
    HUD_StartSlideOut(rec);
    dk::jump(at_HUD_PanelNext);
}
