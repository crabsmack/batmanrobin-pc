#include "game.h"

/* Hitbox_CanHitTarget - hit test for enemy attacks: Hitbox_CanHitPlayer extended to owner id 5 (a
 * box of id 5 goes straight to the owner checks; what owns id 5 is not identified).
 * In:  box = the struck box (a2: +$10 id)
 * Out: as Hitbox_CanHitPlayer: d0 = 0 and Z set when the hit is accepted
 * Evidence: move.w $10(a2),d0 / cmpi.w #5,d0 / beq.s Hitbox_CanHitOwner, else the fall into
 *           Hitbox_CanHitPlayer (which reloads d0); callers the EnemyAtk_* callbacks, $3B6FA,
 *           $3DE62, $41FFC
 */
void Hitbox_CanHitTarget(uint32_t *d0, uint16_t *d1, uint32_t *a1, HitBox *box, dk::Flags *ccr)
{
    (void)d1;
    (void)a1;
    *d0 = (*d0 & 0xFFFF0000u) | box->id;
    *ccr = dk::Flags::compare(box->id, 5, 2, ccr->x);
    if (ccr->z) {
        dk::jump(at_Hitbox_CanHitOwner);
        return;
    }
    dk::jump(at_Hitbox_CanHitPlayer);
}
