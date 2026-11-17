#include "game.h"

/* Enemy_JetTrooper_ShotHit - hit callback of the jetpack trooper's gun shots: the plain body-contact
 * hit (EnemyAtk_HitTouch); after a hit it only loads the struck player's motion block into a1 (no
 * extra effect, unlike Enemy_PuffPlant_PuffHit)
 * In:  box = the struck box (a2: +$2E its owner's handle), self = the shot (a6)
 * Out: EnemyAtk_HitTouch's d0 / d1 / a1; after a hit a1 = the struck owner's motion block (a3)
 * Evidence: jsr EnemyAtk_HitTouch / bne.s rts / movea.w $2E(a2),a1 / movea.w $8(a1),a1; callback long
 *           $000429BC in the a4 shot script $42964 (Op4_HitPlayersOnPhase at $42986); TAS
 *           125825-125837 (5 hits, stage 4 tube section), scenario-level-skip 13475-13774
 */
void Enemy_JetTrooper_ShotHit(uint32_t *d0, uint16_t *d1, uint32_t *a1, HitBox *box, const Vm2Obj *self)
{
    dk::Flags f{};
    EnemyAtk_HitTouch(d0, d1, a1, box, self, &f);
    if (!f.z)
        return;
    const Vm2Obj *owner = dk::handle<const Vm2Obj>(box->owner);
    *a1 = dk::addr(dk::handle(owner->motion));
}
