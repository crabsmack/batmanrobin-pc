#include "game.h"

static constexpr uint32_t Tbl_010F76 = 0x010F76u;   /* credits, slide out, wait, stats, slide in */
static constexpr uint32_t Tbl_010FBE = 0x010FBEu;   /* the zero-credit loop */

/* HUD_PlayerJoined - switches the HUD when a player joins mid-game: the joiner's panel shows the
 * credits, slides out and back with the stats; when no credits are left the other player's panel,
 * if that player is out of play, shows the zero-credit loop.
 * In:  rec = the joining player's record (a0)
 * Out: a1 = the other player's record
 * Evidence: move.w #1,$1E(a0) / move.l #$10F76,$20(a0) / lea g_Player2,a1 / cmpa.l #$FFFFF650,a0
 *           (g_Player1) / beq / lea g_Player1,a1 / btst #0,$25(a1) / tst.w g_Credits / #$10FBE
 */
uint32_t HUD_PlayerJoined(PlayerRec *rec)
{
    rec->panel_timer = 1;
    rec->panel_script = Tbl_010F76;
    PlayerRec *other = (rec == (const PlayerRec *)g_Player1) ? (PlayerRec *)g_Player2 : (PlayerRec *)g_Player1;
    if (!(other->flags & PLAYER_IN_PLAY) && g_Credits == 0) {
        other->panel_timer = 1;
        other->panel_script = Tbl_010FBE;
    }
    return dk::addr(other);
}
