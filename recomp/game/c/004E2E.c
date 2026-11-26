#include "game.h"

/* Hitbox_KindAllEnemies - entry $C of Hitbox_TestAttackBoxAt's kind jump table: attack-box kind $C
 * goes to Hitbox_HitAllEnemyBoxes.
 * In:  as Hitbox_KindFirstEnemy
 * Out: as Hitbox_HitAllEnemyBoxes
 * Evidence: bra.w Hitbox_HitAllEnemyBoxes at $4E2E, the jmp $4E22(pc,d3.w) with d3 = $C; never run
 *           in the corpus. Prototype: Hitbox_KindFirstEnemy's (the same kind of branch)
 */
void Hitbox_KindAllEnemies(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4,
                           uint32_t a5, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    (void)a0, (void)a1, (void)a3, (void)a4, (void)a5, (void)a6;
    dk::jump(at_Hitbox_HitAllEnemyBoxes);
}
