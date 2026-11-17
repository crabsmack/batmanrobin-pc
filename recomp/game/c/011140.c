#include "game.h"

static constexpr uint32_t Tbl_010FD2 = 0x010FD2u;   /* GAME OVER, 1000 frames (the player out) */
static constexpr uint32_t Tbl_010FD8 = 0x010FD8u;   /* GAME OVER (the other, out too) */
static constexpr uint32_t Tbl_010FE0 = 0x010FE0u;   /* GAME OVER, then PRESS START / credits */
static constexpr uint32_t Tbl_011012 = 0x011012u;   /* BOSS TIME / PLEASE WAIT / credits */
static constexpr uint32_t Tbl_010FB2 = 0x010FB2u;   /* GAME OVER, no credits */

/* HUD_PlayerOut - switches the HUD when a player has lost the last life. The other player out
 * too: GAME OVER on both panels (the other's starts next frame) and joining locked. Otherwise the
 * out player's panel: GAME OVER then PRESS START / credits, or BOSS TIME while joining is locked,
 * or GAME OVER alone without credits. The out player's panel waits 4 frames before its script.
 * In:  rec = the record of the player who is out (a0)
 * Out: a1 = the other player's record
 * Evidence: move.w #4,$1E(a0) / lea g_Player2,a1 / cmpa.l #$FFFFF650,a0 (g_Player1) / beq / lea
 *           g_Player1,a1 / btst #0,$25(a1) / the scripts $10FD2/$10FD8, st.b g_JoinLocked, or
 *           tst.w g_Credits / tst.b g_JoinLocked / $10FE0, $11012, $10FB2; one caller $1048E
 */
uint32_t HUD_PlayerOut(PlayerRec *rec)
{
    rec->panel_timer = 4;
    PlayerRec *other = (rec == (const PlayerRec *)g_Player1) ? (PlayerRec *)g_Player2 : (PlayerRec *)g_Player1;
    if (!(other->flags & PLAYER_IN_PLAY)) {
        rec->panel_script = Tbl_010FD2;
        other->panel_script = Tbl_010FD8;
        other->panel_timer = 1;
        g_JoinLocked = 0xFF;
    } else if (g_Credits == 0) {
        rec->panel_script = Tbl_010FB2;
    } else if (g_JoinLocked) {
        rec->panel_script = Tbl_011012;
    } else {
        rec->panel_script = Tbl_010FE0;
    }
    return dk::addr(other);
}

/* HUD_LockJoin - BOSS TIME: locks joining and, when credits are left, puts both players through
 * HUD_ShowBossTime (the absent ones' panels read BOSS TIME / PLEASE WAIT).
 * In:  none
 * Out: g_JoinLocked = $FF
 * Evidence: st.b (g_JoinLocked).w / tst.w g_Credits / beq HUD_BossTimeReturn (an rts) / lea
 *           g_Player1,a0 / bsr HUD_ShowBossTime / lea g_Player2,a0 and the fall into
 *           HUD_ShowBossTime (a jump: a0, a clobber, is an output here so the jump hands it on); 14 corpus runs
 */
void HUD_LockJoin(uint32_t *a0)
{
    g_JoinLocked = 0xFF;
    if (g_Credits == 0)
        return;
    HUD_ShowBossTime((PlayerRec *)g_Player1);
    *a0 = dk::addr(g_Player2);
    dk::jump(at_HUD_ShowBossTime);
}
