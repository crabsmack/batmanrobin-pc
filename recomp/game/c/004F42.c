#include "game.h"

/* Hitbox_LinkByKind_AllPlayers - Hitbox_LinkByKind's entry for kind 4: appends the box to
 * g_PlayerBoxList.
 * In:  a0 = the box; a1 passed on
 * Out: the box linked (Hitbox_LinkPlayerList's rts)
 * Evidence: bra.w Hitbox_LinkPlayerList at $004F42; jsr $4F3E(pc,d3.w) with d3 = 4; never run in the corpus
 */
void Hitbox_LinkByKind_AllPlayers(uint32_t a0, uint32_t a1)
{
    (void)a0, (void)a1;
    dk::jump(at_Hitbox_LinkPlayerList);
}
