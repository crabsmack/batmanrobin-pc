#include "game.h"

static constexpr uint32_t Tbl_011012 = 0x011012u;   /* BOSS TIME / PLEASE WAIT / credits loop */

/* HUD_ShowBossTime - puts one absent player's panel on the BOSS TIME / PLEASE WAIT / credits
 * loop; a player in play is left alone.
 * In:  rec = the player record (a0)
 * Out: +$20 = Tbl_011012 and +$1E = 1 when not in play
 * Evidence: btst #0,$25(a0) / bne HUD_BossTimeReturn / move.l #$11012,$20(a0) / move.w #1,$1E(a0);
 *           the per-record half of HUD_LockJoin
 */
void HUD_ShowBossTime(PlayerRec *rec)
{
    if (rec->flags & PLAYER_IN_PLAY) {
        return;
    }
    rec->panel_script = Tbl_011012;
    rec->panel_timer = 1;
}

/* HUD_BossTimeReturn - the shared rts of HUD_LockJoin and HUD_ShowBossTime.
 * In:  none
 * Out: none
 * Evidence: loc_0111D8: rts, branched to by HUD_LockJoin (no credits) and HUD_ShowBossTime (in play)
 */
void HUD_BossTimeReturn(void)
{
}

/* HUD_UnlockJoin - ends BOSS TIME: unless the frame callback's high word is 2, unlocks joining
 * and, when credits are left, puts the absent players' panels back on PRESS START.
 * In:  none
 * Out: g_JoinLocked = 0; the flags: the cmpi's (callback 2), the tst.w of g_Credits (none), or
 *      HUD_ShowPressStart's for player 2
 * Evidence: cmpi.w #2,(g_FrameCallback).w (the word at $F604: the long's high word) / beq
 *           HUD_PressStartReturn / sf.b g_JoinLocked / tst.w g_Credits / beq / lea g_Player1,a0 /
 *           bsr HUD_ShowPressStart / lea g_Player2,a0 and the fall into HUD_ShowPressStart
 *           (a jump: a0, a clobber, is an output here so the jump hands it on); reached by bra from $11B06
 */
void HUD_UnlockJoin(uint32_t *a0, dk::Flags *ccr)
{
    uint16_t callback_hi = (uint16_t)(g_FrameCallback >> 16);
    *ccr = dk::Flags::compare(callback_hi, 2, 2, ccr->x);
    if (ccr->z)
        return;
    g_JoinLocked = 0;
    *ccr = dk::Flags::test(g_Credits, 2, ccr->x);
    if (ccr->z)
        return;
    HUD_ShowPressStart((PlayerRec *)g_Player1, ccr);
    *a0 = dk::addr(g_Player2);
    dk::jump(at_HUD_ShowPressStart);
}
