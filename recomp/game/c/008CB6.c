#include "game.h"

/* Hitbox_FindPlayerBoxOfId - starts an id scan of g_PlayerBoxList: the first box whose id (+$10)
 * is d3 and whose rectangle overlaps d4-d7, tested by Hitbox_TestBoxOfId (the next file's loop).
 * In:  d3 = the box id; d4 left, d5 top, d6 right, d7 bottom
 * Out: a2 = the first node (sign-extended); an empty list returns Z set (tst.w (a2)); else
 *      Hitbox_TestBoxOfId's d0 / a2 / Z
 * Evidence: movea.w g_PlayerBoxList,a2 / tst.w (a2) / bne.s Hitbox_TestBoxOfId / rts; only caller
 *           $3434 (Op2_BranchPlayerInBox). Prototype: a2 an output (it was a clobber): the jump
 *           hands the first node to Hitbox_TestBoxOfId
 */
bool Hitbox_FindPlayerBoxOfId(uint32_t *d0, uint16_t d3, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7,
                              uint32_t *a2)
{
    (void)d0, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    *a2 = (uint32_t)(int32_t)(int16_t)g_PlayerBoxList;
    if (dk::handle<HitBox>((int16_t)g_PlayerBoxList)->next != 0) {
        dk::jump(at_Hitbox_TestBoxOfId);
        return false;
    }
    return true;   /* Z: nothing on the list */
}
