#include "game.h"

/* the sprite attribute bit for a mirrored (facing left) object: bit 3 of the attribute's high byte */
static constexpr uint16_t ATTR_FLIP_H = 0x0800;

/* EnemyAtk_ApplyHit - common tail of EnemyAtk_HitPlayerA/B: the attack's damage to the struck
 * box, its knockback (+$36 x 256, negated when the attacker faces left), hit type 7 (knockback), a
 * 7-run movement pause for the attacker, then the spark a1 at the contact point.
 * In:  attack = the attack box (a0); spark = the a4 script (a1); box = the struck box (a2);
 *      mot/spr = the attacker's motion and sprite blocks (a3/a5); d4-d7 = the attack rectangle
 * Out: d0 = the knockback; then Hitbox_SpawnAtContact's
 * Evidence: move.w $34(a0),d0 / add.w d0,$20(a2) / move.w $36(a0),d0 / ext.l / asl.l #8 / btst #3,
 *           $38(a5) / neg.l / move.l d0,$22(a2) / move.b #7,$1(a3) / move.w #7,$1A(a2) / bra.w
 *           Hitbox_SpawnAtContact
 */
void EnemyAtk_ApplyHit(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                       uint32_t d7, const HitBox *attack, uint32_t spark, HitBox *box, Vm2Motion *mot, uint32_t *a4,
                       const Vm2Sprite *spr, uint32_t *a6)
{
    (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)spark, (void)a4, (void)a6;
    box->damage += attack->hit_damage;
    int32_t knockback = (int32_t)((uint32_t)(int32_t)(int16_t)attack->hit_knockback << 8);
    if (spr->attr & ATTR_FLIP_H)
        knockback = -knockback;
    *d0 = (uint32_t)knockback;
    box->knockback = knockback;
    mot->stun = 7;
    box->hit_type = 7;
    dk::jump(at_Hitbox_SpawnAtContact);
}
