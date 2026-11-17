#include "game.h"

enum {
    PUFF_STUN = 2,        /* runs the struck player's velocity stays frozen */
    PUFF_LIFT = -4,       /* its y speed, px a frame (the integer word of the 16.16 velocity) */
};

/* Enemy_VentTank_PuffHit - hit callback of the gas puffs the vent tank blows up from its nozzles:
 * EnemyAtk_HitTouchSpark, then the struck box's owner gets a 2-frame stun and y speed -4 px a frame
 * (blown upward). The asm doesn't test the hit's result: the stun and lift happen on every call,
 * hit or not (kept).
 * In:  box = the struck box (a2: +$2E its owner's handle), a6 = the puff; the rest as
 *      EnemyAtk_HitTouchSpark
 * Out: a1 = the struck owner's motion block; d0-d7 / a0 / a4 / a6 as EnemyAtk_HitTouchSpark left them
 * Evidence: jsr EnemyAtk_HitTouchSpark / movea.w $2E(a2),a1 / movea.w $8(a1),a1 / move.b #2,1(a1) /
 *           move.w #-4,$1C(a1) / rts; callback long $00042FA6 at $42F70 in the a4 puff script $42F4E
 *           (Op4_HitPlayersOnPhase, damage $10); TAS 121172-121506 (7 hits)
 */
void Enemy_VentTank_PuffHit(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, HitBox *box, uint32_t a3,
                            uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    dk::Flags f{};
    EnemyAtk_HitTouchSpark(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, box, a3, a4, a5, a6, &f);
    const Vm2Obj *owner = dk::handle<const Vm2Obj>(box->owner);
    Vm2Motion *motion = dk::handle<Vm2Motion>(owner->motion);
    *a1 = dk::addr(motion);
    motion->stun = PUFF_STUN;
    /* move.w to +$1C: only the integer word of the 16.16 y velocity; the fraction stays */
    uint32_t vel_y = (uint32_t)(int32_t)motion->vel_y;
    motion->vel_y = (int32_t)(((uint32_t)(uint16_t)(int16_t)PUFF_LIFT << 16) | (vel_y & 0xFFFFu));
}
