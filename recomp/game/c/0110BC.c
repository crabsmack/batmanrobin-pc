#include "game.h"

/* the panel scripts (HUD_RunPanelScript) */
static constexpr uint32_t Tbl_010F84 = 0x010F84u;   /* show the stats, slide in */
static constexpr uint32_t Tbl_010F8A = 0x010F8Au;   /* slide in, GAME OVER, PRESS START / credits loop */
static constexpr uint32_t Tbl_010FB6 = 0x010FB6u;   /* GAME OVER + the no-credits loop */

/* HUD_InitPanel - starts one player's HUD panel script: the stats when the player is in play,
 * otherwise GAME OVER with PRESS START (credits left) or without.
 * In:  rec = the player record (a0)
 * Out: +$1E = 1 (the script steps next frame), +$1A text cleared, +$24/+$3A cleared, text mode on,
 *      +$20 = the script
 * Evidence: move.w #1,$1E(a0) / clr.l $1A(a0) / clr.b $24(a0) / clr.b $3A(a0) / bset #7,$25(a0) /
 *           btst #0 / move.l #$10F84 or (tst.w g_Credits) #$10F8A / #$10FB6, $20(a0)
 */
void HUD_InitPanel(PlayerRec *rec)
{
    rec->panel_timer = 1;
    rec->panel_text = 0;
    rec->ring_anim = 0;
    rec->targetable = 0;
    rec->flags = (uint8_t)(rec->flags | PLAYER_PANEL_TEXT);
    if (rec->flags & PLAYER_IN_PLAY)
        rec->panel_script = Tbl_010F84;
    else if (g_Credits)
        rec->panel_script = Tbl_010F8A;
    else
        rec->panel_script = Tbl_010FB6;
}
