#include "game.h"

/* The attack-box searches ($004E32-$004E51): the first (or every) box on a collision list that
 * overlaps the attack rectangle is struck (Hitbox_Strike / Hitbox_StrikeAll, the next file). */

/* the box searches return Z clear with a2 = the box found, or Z set with a2 = the list's tail
   sentinel: the one node whose next link is 0 (the search tests only nodes with a non-zero link),
   so the link says which (bne after the bsr) */
static bool found_box(uint32_t a2)
{
    return dk::handle<HitBox>((int16_t)(uint16_t)a2)->next != 0;
}

/* Hitbox_HitFirstPlayerBox - attack-box kind 0: strikes the first box on g_PlayerBoxList that
 * overlaps the rectangle, if any.
 * In:  d4-d7 = the rectangle; a0 = the attack box; a6 = the owner
 * Out: a2 = the box found; a hit goes on at Hitbox_Strike, else returns
 * Evidence: bsr.w Hitbox_FindPlayerBox / bne.s Hitbox_Strike / rts; 32 corpus runs. Prototype: a2
 *           an output (it was a clobber): the jump hands the found box to Hitbox_Strike
 */
void Hitbox_HitFirstPlayerBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t *a2, uint32_t a3,
                              uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    (void)d1, (void)d2, (void)d3, (void)a0, (void)a1, (void)a3, (void)a4, (void)a5, (void)a6;
    Hitbox_FindPlayerBox(d0, (uint16_t)*d4, (uint16_t)*d5, (uint16_t)*d6, (uint16_t)*d7, a2);
    if (found_box(*a2))
        dk::jump(at_Hitbox_Strike);
}

/* Hitbox_HitAllPlayerBoxes - attack-box kind 4: strikes every box on g_PlayerBoxList that
 * overlaps the rectangle.
 * In:  d4-d7 = the rectangle; a0 = the attack box; a6 = the owner
 * Out: a2 = the first box found; a hit goes on at Hitbox_StrikeAll, else returns
 * Evidence: bsr.w Hitbox_FindPlayerBox / bne.s Hitbox_StrikeAll / rts; jump-table entry 4 ($4E26).
 *           Prototype: a2 an output, as Hitbox_HitFirstPlayerBox
 */
void Hitbox_HitAllPlayerBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t *a2, uint32_t a3,
                              uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    (void)d1, (void)d2, (void)d3, (void)a0, (void)a1, (void)a3, (void)a4, (void)a5, (void)a6;
    Hitbox_FindPlayerBox(d0, (uint16_t)*d4, (uint16_t)*d5, (uint16_t)*d6, (uint16_t)*d7, a2);
    if (found_box(*a2))
        dk::jump(at_Hitbox_StrikeAll);
}

/* Hitbox_HitFirstEnemyBox - attack-box kind 8: strikes the first box on g_EnemyBoxList that
 * overlaps the rectangle (the players' attacks).
 * In:  d4-d7 = the rectangle; a0 = the attack box; a6 = the owner
 * Out: a2 = the box found; a hit goes on at Hitbox_Strike, else returns
 * Evidence: bsr.w Hitbox_FindEnemyBox / bne.s Hitbox_Strike / rts; jump-table entry 8 ($4E2A).
 *           Prototype: a2 an output, as Hitbox_HitFirstPlayerBox
 */
void Hitbox_HitFirstEnemyBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t *a2, uint32_t a3,
                             uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    (void)d1, (void)d2, (void)d3, (void)a0, (void)a1, (void)a3, (void)a4, (void)a5, (void)a6;
    Hitbox_FindEnemyBox(d0, (uint16_t)*d4, (uint16_t)*d5, (uint16_t)*d6, (uint16_t)*d7, a2);
    if (found_box(*a2))
        dk::jump(at_Hitbox_Strike);
}
