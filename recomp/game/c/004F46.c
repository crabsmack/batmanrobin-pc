#include "game.h"

/* Hitbox_LinkByKind_Enemy - Hitbox_LinkByKind's entry for kind 8: appends the box to
 * g_EnemyBoxList.
 * In:  a0 = the box; a1 passed on
 * Out: the box linked (Hitbox_LinkEnemyList's rts)
 * Evidence: bra.w Hitbox_LinkEnemyList; reached by jsr $4F3E(pc,d3.w) with d3 = 8 (138 corpus runs)
 */
void Hitbox_LinkByKind_Enemy(uint32_t a0, uint32_t a1)
{
    (void)a0, (void)a1;
    dk::jump(at_Hitbox_LinkEnemyList);
}
