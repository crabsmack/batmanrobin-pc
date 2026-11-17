#include "game.h"

enum {
    DEBRIS_HIT_PAUSE = 0x64,   /* frames the debris's own attacks pause after a hit */
    DEBRIS_HIT_SHAKE = 8,      /* frames its sprite shakes */
};

/* Enemy_ArenaDebris_HitPlayer - attack-box callback of the debris tumbling into the cat-head
 * arena: when Hitbox_CanHitTarget accepts the struck box, pauses the debris's attacks for 100
 * frames, shakes its sprite 8 frames, turns a fall (y velocity >= 0) into a bounce at a quarter
 * of the speed, then knocks the player back (EnemyAtk_ApplyKnockSpeed)
 * In:  a0 = the attack box (+$34 damage $20, +$36 knockback $200), struck = the struck box (a2),
 *      motion/a4/sprite = the debris's motion, VM and sprite blocks (a3/a4/a5)
 * Out: refused: Hitbox_CanHitTarget's d0 (nonzero) and d1; accepted: d0 = the y velocity (the
 *      bounced one when it fell), then EnemyAtk_ApplyKnockSpeed's results
 * Evidence: jsr Hitbox_CanHitTarget / beq.w / rts / move.b #$64,$1(a4) / move.b #8,$17(a5) /
 *           move.l $1C(a3),d0 / bmi / asr.l #2 / neg.l / move.l d0,$1C(a3) / jmp
 *           EnemyAtk_ApplyKnockSpeed; box spec `... 0003B6FA 0020 0200` at $03B614 in the debris
 *           body $03B5EC; TAS f88206-f88462 (16 frames)
 */
void Enemy_ArenaDebris_HitPlayer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                 uint32_t *d6, uint32_t *d7, uint32_t *a0, HitBox *struck, Vm2Motion *motion,
                                 uint32_t *a4, Vm2Sprite *sprite, uint32_t *a6)
{
    uint16_t d1w = (uint16_t)*d1;
    uint32_t a1 = 0;
    dk::Flags ccr{};
    Hitbox_CanHitTarget(d0, &d1w, &a1, struck, &ccr);
    *d1 = (*d1 & 0xFFFF0000u) | d1w;
    if (!ccr.z)
        return;
    PTR(Vm2Block) vm;
    vm.v = *a4;
    vm->delay = DEBRIS_HIT_PAUSE;
    sprite->shake = DEBRIS_HIT_SHAKE;
    int32_t vy = motion->vel_y;
    if (vy >= 0) {
        vy = (int32_t)(0u - (uint32_t)(vy >> 2));   /* asr.l #2 / neg.l: up at a quarter speed */
        motion->vel_y = vy;
    }
    *d0 = (uint32_t)vy;
    dk::jump(at_EnemyAtk_ApplyKnockSpeed);
}
