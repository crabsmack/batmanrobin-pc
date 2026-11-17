#include "game.h"

/* Player_SetExitFlag - for one player in play: sets bit $800 of its script variable 1 (the player
 * scripts walk off screen) and installs SceneCb_WaitPlayersExit as the frame callback.
 * In:  rec = the player record (a0)
 * Out: the condition codes: not in play = btst's Z (N, V, C as they came in); in play = those of
 *      the move.l of the callback (all clear)
 * Evidence: btst #0,$25(a0) / beq / movea.w $2C(a0),a0 / movea.w $A(a0),a0 / ori.w #$800,$24(a0) /
 *           move.l #$9D96,(g_FrameCallback).w; tail of Section_StartExit(N)
 */
void Player_SetExitFlag(const PlayerRec *rec, dk::Flags *ccr)
{
    ccr->z = !(rec->flags & PLAYER_IN_PLAY);
    if (ccr->z)
        return;
    const Vm2Obj *obj = dk::handle<const Vm2Obj>(rec->object);
    Vm2Block *vm = dk::handle<Vm2Block>(obj->vm);
    vm->var[1] |= 0x800;
    g_FrameCallback = at_SceneCb_WaitPlayersExit;
    *ccr = dk::Flags::test(at_SceneCb_WaitPlayersExit, 4, ccr->x);
}

/* Player_SetBits300All - sets bits $300 in g_PlayMode and in script variable 1 of each player in
 * play (what the pair means is not identified; Player_CaseStepFrame reads $100/$200 as stepping
 * directions).
 * In:  none
 * Out: none
 * Evidence: ori.w #$300,(g_PlayMode).w / lea g_Player1,a0 / bsr Player_SetBits300 / lea g_Player2,a0
 *           and the fall into Player_SetBits300 (a jump: a0, a clobber, is an output here so the jump hands it on); only caller $2417E
 */
void Player_SetBits300All(uint32_t *a0)
{
    g_PlayMode |= 0x300;
    Player_SetBits300((PlayerRec *)g_Player1);
    *a0 = dk::addr(g_Player2);
    dk::jump(at_Player_SetBits300);
}
