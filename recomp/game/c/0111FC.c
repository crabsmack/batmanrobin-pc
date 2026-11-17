#include "game.h"

static constexpr uint32_t Tbl_010FF6 = 0x010FF6u;   /* PRESS START / GAME OVER loop */

/* HUD_ShowPressStart - puts one absent player's panel on the PRESS START / GAME OVER loop; a
 * player in play is left alone.
 * In:  rec = the player record (a0)
 * Out: +$20 = Tbl_010FF6 and +$1E = 1 when not in play; the flags: btst's Z (N, V, C as they came
 *      in) when in play, else the move.w #1's
 * Evidence: btst #0,$25(a0) / bne HUD_PressStartReturn / move.l #$10FF6,$20(a0) / move.w
 *           #1,$1E(a0); the per-record half of HUD_UnlockJoin
 */
void HUD_ShowPressStart(PlayerRec *rec, dk::Flags *ccr)
{
    ccr->z = !(rec->flags & PLAYER_IN_PLAY);
    if (!ccr->z) {
        return;
    }
    rec->panel_script = Tbl_010FF6;
    rec->panel_timer = 1;
    *ccr = dk::Flags::test(1, 2, ccr->x);
}

/* HUD_PressStartReturn - the shared rts of HUD_UnlockJoin and HUD_ShowPressStart.
 * In:  none
 * Out: none
 * Evidence: loc_011212: rts, branched to by HUD_UnlockJoin and HUD_ShowPressStart (in play)
 */
void HUD_PressStartReturn(void)
{
}
