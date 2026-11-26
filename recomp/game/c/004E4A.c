#include "game.h"

/* the box searches return Z clear with a2 = the box found, or Z set with a2 = the list's tail
   sentinel (004E32.c's found_box) */
static bool found_box(uint32_t a2)
{
    return dk::handle<HitBox>((int16_t)(uint16_t)a2)->next != 0;
}

/* Hitbox_HitAllEnemyBoxes - attack-box kind $C: strikes every box on g_EnemyBoxList that overlaps
 * the rectangle.
 * In:  d4-d7 = the rectangle; a0 = the attack box; a6 = the owner
 * Out: a2 = the first box found; a hit goes on at Hitbox_StrikeAll, else returns
 * Evidence: bsr.w Hitbox_FindEnemyBox / bne.s Hitbox_StrikeAll / rts, Hitbox_HitAllPlayerBoxes on
 *           the enemy list; never run in the corpus. Prototype: Hitbox_HitAllPlayerBoxes'
 */
void Hitbox_HitAllEnemyBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t *a2, uint32_t a3,
                             uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    (void)d1, (void)d2, (void)d3, (void)a0, (void)a1, (void)a3, (void)a4, (void)a5, (void)a6;
    Hitbox_FindEnemyBox(d0, (uint16_t)*d4, (uint16_t)*d5, (uint16_t)*d6, (uint16_t)*d7, a2);
    if (found_box(*a2))
        dk::jump(at_Hitbox_StrikeAll);
}
