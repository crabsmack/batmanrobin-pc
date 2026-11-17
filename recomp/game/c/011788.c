#include "game.h"

/* HudOp_Halt - panel script op 0: stops the script on this op (the timer 0 ends
 * HUD_RunPanelScript's stepping; the pointer stays on the halt).
 * In:  a0 = the script after the op word; rec = the player record (a6)
 * Out: +$20 = the op itself, +$1E = 0
 * Evidence: lea -2(a0),a0 / move.l a0,$20(a6) / clr.w $1E(a6) / rts; entry 0 of Tbl_011764; ends
 *           the stats script $10F84 and the GAME OVER script $10FD2
 */
void HudOp_Halt(uint32_t a0, PlayerRec *rec)
{
    rec->panel_script = a0 - 2;
    rec->panel_timer = 0;
}
