#include "game.h"

/* Hitbox_LinkByKind_AllEnemies - Hitbox_LinkByKind's entry for kind $C: appends the box to
 * g_EnemyBoxList.
 * In:  a0 = the box; a1 passed on
 * Out: the box linked (Hitbox_LinkEnemyList's rts)
 * Evidence: bra.w Hitbox_LinkEnemyList at $004F4A; jsr $4F3E(pc,d3.w) with d3 = $C; never run in the corpus
 */
void Hitbox_LinkByKind_AllEnemies(uint32_t a0, uint32_t a1)
{
    (void)a0, (void)a1;
    dk::jump(at_Hitbox_LinkEnemyList);
}
