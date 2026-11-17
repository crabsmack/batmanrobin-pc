#include "game.h"

/* Player_GetP1Var1 - if player 1 is in play, its object's script variable 1.
 * In:  g_Player1 (+$25 bit 0 = in play, +$2C = the object's handle)
 * Out: in play: var (d0) = the VM block's variable 1, one (d1) = 1, vm (a4) = the object's VM
 *      block, ccr Z clear (moveq #1 last); not in play: Z set, the rest unchanged
 * Evidence: lea g_Player1,a0 / btst #0,$25(a0) / beq / movea.w $2C(a0),a6 / movea.w $A(a6),a4 /
 *           move.w $24(a4),d0 / moveq #1,d1; only caller SceneCb_WaitPlayersExit
 */
void Player_GetP1Var1(uint16_t *var, uint32_t *one, uint32_t *vm, dk::Flags *ccr)
{
    const PlayerRec *p = (const PlayerRec *)g_Player1;
    *ccr = dk::Flags::test(p->flags & 1, 1, ccr->x);       /* btst: Z = the bit clear */
    if (ccr->z)
        return;
    const Vm2Obj *obj = dk::handle<const Vm2Obj>(p->object);
    const Vm2Block *block = dk::handle<const Vm2Block>(obj->vm);
    *vm = dk::addr(block);
    *var = block->var[1];
    *one = 1;
    *ccr = dk::Flags::test(1, 4, ccr->x);
}
