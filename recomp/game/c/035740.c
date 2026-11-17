#include "game.h"

enum {
    PUFF_STUN_RUNS = 0x0A,   /* runs the struck player's motion stays frozen */
    PUFF_LIFT_VEL_Y = -4,    /* y velocity given (whole pixels per run: blown upward) */
};

/* Enemy_PuffPlant_PuffHit - hit callback of the puff plant's rising puffs: EnemyAtk_HitTouch, and
 * when it hit, the struck player's motion block gets a 10-run stun and y speed -4 px per run
 * In:  box = the struck box (a2: +$2E its owner); self = the touching puff (a6); d0/d1/a1 as
 *      EnemyAtk_HitTouch takes them
 * Out: EnemyAtk_HitTouch's d0/d1/a1; after a hit a1 = the owner's motion block (sign-extended
 *      handle), its stun = $A and its y velocity's integer part = -4 (the fraction kept)
 * Evidence: jsr EnemyAtk_HitTouch / bne / movea.w $2E(a2),a1 / movea.w $8(a1),a1 / move.b #$A,1(a1) /
 *           move.w #$FFFC,$1C(a1); callback long $00035740 in the puff a4 script $356EE at $3570A;
 *           TAS f96393-f96411 (4 hits)
 */
void Enemy_PuffPlant_PuffHit(uint32_t *d0, uint16_t *d1, uint32_t *a1, HitBox *box, const Vm2Obj *self)
{
    dk::Flags f{};
    EnemyAtk_HitTouch(d0, d1, a1, box, self, &f);
    if (!f.z)
        return;   /* no hit */
    const Vm2Obj *struck = dk::handle<const Vm2Obj>(box->owner);
    Vm2Motion *motion = dk::handle<Vm2Motion>(struck->motion);
    *a1 = dk::addr(motion);
    motion->stun = PUFF_STUN_RUNS;
    /* move.w to the high word: the integer part only */
    motion->vel_y = (int32_t)(((uint32_t)(uint16_t)PUFF_LIFT_VEL_Y << 16) | ((uint32_t)motion->vel_y & 0xFFFFu));
}
