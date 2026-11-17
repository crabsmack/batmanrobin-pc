#include "game.h"

/* HudOp_SlideIn - panel script op $10: starts the panel sliding back in from its screen edge and
 * runs on.
 * In:  a0 = the next op; rec = the player record (a6)
 * Out: +$E/+$12/+$16 the slide, +$25 bit 6 cleared; d0/d4/d5 as HUD_StartSlideIn leaves them; then
 *      HUD_PanelNext
 * Evidence: bsr.w HUD_StartSlideIn / bra.s HUD_PanelNext; entry $10 of Tbl_011764; right after the
 *           content switch in $10F84 and $10F8A
 */
void HudOp_SlideIn(uint32_t *d0, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a1, PlayerRec *rec)
{
    (void)a0, (void)a1;
    HUD_StartSlideIn(d0, d4, d5, rec);
    dk::jump(at_HUD_PanelNext);
}
