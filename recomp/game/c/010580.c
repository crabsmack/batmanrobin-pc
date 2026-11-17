#include "game.h"

/* Player_SetBits300 - sets bits $300 of one in-play player's script variable 1.
 * In:  rec = the player record (a0)
 * Out: none
 * Evidence: btst #0,$25(a0) / beq / movea.w $2C(a0),a0 / movea.w $A(a0),a0 / ori.w #$300,$24(a0);
 *           TAS frames 33525-35313
 */
void Player_SetBits300(const PlayerRec *rec)
{
    if (!(rec->flags & PLAYER_IN_PLAY))
        return;
    const Vm2Obj *obj = dk::handle<const Vm2Obj>(rec->object);
    dk::handle<Vm2Block>(obj->vm)->var[1] |= 0x300;
}

/* Player_ClearBits300All - undoes Player_SetBits300All: clears bits $300 in g_PlayMode and in
 * script variable 1 of each player in play.
 * In:  none
 * Out: none
 * Evidence: andi.w #$FCFF,(g_PlayMode).w / lea g_Player1,a0 / bsr Player_ClearBits300 / lea
 *           g_Player2,a0 and the fall into Player_ClearBits300 (a jump: a0, a clobber, is an output here so the jump hands it on); only caller $2417E
 */
void Player_ClearBits300All(uint32_t *a0)
{
    g_PlayMode &= 0xFCFF;
    Player_ClearBits300((PlayerRec *)g_Player1);
    *a0 = dk::addr(g_Player2);
    dk::jump(at_Player_ClearBits300);
}
