#include "game.h"

/* Item_BoxPlayer - the player record of a collision box's owner (owner id 1 or 2); any other
 * owner: the return address is dropped, so the calling native ends too (the return goes to its
 * caller).
 * In:  who = the box
 * Out: who = the box's owner object; player = g_Player1 or g_Player2; flags = the owner id compared
 *      with 1, then 2 (the callers branch on them)
 * Evidence: movea.w $2E(a1),a1 / cmpi.w #1,$58(a1) / beq / cmpi.w #2 / beq / addq.l #4,a7 / rts;
 *           lea g_Player1 / g_Player2
 */
void Item_BoxPlayer(PlayerRec **player, const void **who, dk::Flags *flags)
{
    const Vm2Obj *owner = dk::handle<const Vm2Obj>(((const HitBox *)*who)->owner);
    *who = owner;
    *flags = dk::Flags::compare(owner->owner_id, 1, 2, flags->x);
    if (flags->z) {
        *player = (PlayerRec *)g_Player1;
        return;
    }
    *flags = dk::Flags::compare(owner->owner_id, 2, 2, flags->x);
    if (flags->z) {
        *player = (PlayerRec *)g_Player2;
        return;
    }
    dk::drop_return();
}
