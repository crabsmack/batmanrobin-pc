#include "game.h"

/* Hitbox_FindEnemyBox - starts an overlap scan of g_EnemyBoxList: the first box overlapping the
 * rectangle, tested by Hitbox_TestBox.
 * In:  d4 left, d5 top, d6 right, d7 bottom (world pixels)
 * Out: a2 = the first node (sign-extended); an empty list returns Z set (tst.w (a2)); else
 *      Hitbox_TestBox's d0 / a2 / Z
 * Evidence: movea.w g_EnemyBoxList,a2 / tst.w (a2) / bne.s Hitbox_TestBox / rts; callers $3768,
 *           $4E42, $662C, $69AE
 */
bool Hitbox_FindEnemyBox(uint32_t *d0, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint32_t *a2)
{
    (void)d0, (void)d4, (void)d5, (void)d6, (void)d7;
    *a2 = (uint32_t)(int32_t)(int16_t)g_EnemyBoxList;
    if (dk::handle<HitBox>((int16_t)g_EnemyBoxList)->next != 0) {
        dk::jump(at_Hitbox_TestBox);
        return false;
    }
    return true;   /* Z: nothing on the list */
}
