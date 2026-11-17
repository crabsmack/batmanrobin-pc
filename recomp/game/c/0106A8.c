#include "game.h"

/* The body-contact and knockdown hit callbacks of the enemies ($0106A8-$010721). */

static constexpr uint32_t Scr4_010964 = 0x010964u;   /* the hit spark */

/* EnemyAtk_HitTouch - enemy body-contact hit callback: when Hitbox_CanHitTarget accepts the box,
 * adds the attacker's touch damage and a flinch (hit type 6), no spark.
 * In:  box = the struck box (a2); self = the attacker (a6: +$12 touch damage)
 * Out: d0 = 0, Z set after a hit (moveq); else Hitbox_CanHitTarget's d0 and flags
 * Evidence: bsr Hitbox_CanHitTarget / bne / move.w $12(a6),d0 / add.w d0,$20(a2) / move.w #6,$1A(a2)
 *           / moveq #0,d0; referenced at 21 sites ($1506C-$151C6, the a4 native op $5438 at $1ABAE)
 */
void EnemyAtk_HitTouch(uint32_t *d0, uint16_t *d1, uint32_t *a1, HitBox *box, const Vm2Obj *self, dk::Flags *ccr)
{
    Hitbox_CanHitTarget(d0, d1, a1, box, ccr);
    if (!ccr->z)
        return;
    box->damage += self->touch_damage;
    box->hit_type = 6;
    *d0 = 0;
    *ccr = dk::Flags::test(0, 4, ccr->x);
}

/* EnemyAtk_HitTouchSpark - EnemyAtk_HitTouch plus the hit spark Scr4_010964 at the contact point.
 * In:  as EnemyAtk_HitTouch; d4-d7 = the attack rectangle; a3/a5 passed to the spark
 * Out: d0 = 0, Z set after a hit; d1/d2 as Hitbox_SpawnAtContact left them; a1 = the spark
 * Evidence: bsr Hitbox_CanHitTarget / bne / the HitTouch damage / lea Scr4_010964,a1 / bsr
 *           Hitbox_SpawnAtContact / moveq #0,d0; callback long at $3029A, $359FC, jsr from $42FA6
 */
void EnemyAtk_HitTouchSpark(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, HitBox *box, uint32_t a3,
                            uint32_t *a4, uint32_t a5, uint32_t *a6, dk::Flags *ccr)
{
    uint16_t owner_id = (uint16_t)*d1;
    Hitbox_CanHitTarget(d0, &owner_id, a1, box, ccr);
    *d1 = (*d1 & 0xFFFF0000u) | owner_id;
    if (!ccr->z)
        return;
    PTR(const Vm2Obj) self;
    self.v = *a6;
    uint16_t damage = self->touch_damage;
    *d0 = (*d0 & 0xFFFF0000u) | damage;
    box->damage += damage;
    box->hit_type = 6;
    *a1 = Scr4_010964;
    Hitbox_SpawnAtContact(d0, d1, d2, *d3, *d4, *d5, *d6, *d7, *a0, *a1, box, a3, a4, a5, a6);
    *d0 = 0;
    *ccr = dk::Flags::test(0, 4, ccr->x);
}

/* The knockdown tail ($0106F0): when the box can be hit, the touch damage, hit type 9 and the push
 * d2 pointed away from the attack box (negated when the struck box's doubled centre is left of
 * x1 + x2), then the spark at the contact point. */
static void hit_knockdown(uint32_t push, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d4, uint32_t d6,
                          uint32_t *a1, HitBox *box, uint32_t a6)
{
    *d2 = push;
    uint16_t owner_id = (uint16_t)*d1;
    dk::Flags f{};
    Hitbox_CanHitTarget(d0, &owner_id, a1, box, &f);
    *d1 = (*d1 & 0xFFFF0000u) | owner_id;
    if (!f.z)
        return;
    PTR(const Vm2Obj) self;
    self.v = a6;
    uint16_t damage = self->touch_damage;
    *d0 = (*d0 & 0xFFFF0000u) | damage;
    *a1 = Scr4_010964;
    box->damage += damage;
    box->hit_type = 9;
    uint16_t diff = (uint16_t)((uint16_t)box->left + (uint16_t)box->right - (uint16_t)d4 - (uint16_t)d6);
    *d1 = (*d1 & 0xFFFF0000u) | diff;
    if ((int16_t)diff < 0)
        *d2 = 0u - *d2;
    box->knockback = (int32_t)*d2;
    dk::jump(at_Hitbox_SpawnAtContact);
}

/* EnemyAtk_HitKnockdown3 - enemy hit callback: touch damage, knockdown (hit type 9) with a 3.0
 * push away from the attack, spark Scr4_010964.
 * In:  as EnemyAtk_HitTouchSpark
 * Out: d0 nonzero when no hit (Hitbox_CanHitTarget's); after a hit Hitbox_SpawnAtContact's
 * Evidence: move.l #$30000,d2 / bra.s $0106F0 (bsr Hitbox_CanHitTarget / beq / rts, then the
 *           tail and bra.w Hitbox_SpawnAtContact); callback long at $26B9A, $3B84A
 */
void EnemyAtk_HitKnockdown3(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5,
                            uint32_t d6, uint32_t d7, uint32_t a0, uint32_t *a1, HitBox *box, uint32_t a3,
                            uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    (void)d3, (void)d5, (void)d7, (void)a0, (void)a3, (void)a4, (void)a5;
    hit_knockdown(0x30000u, d0, d1, d2, d4, d6, a1, box, *a6);
}

/* EnemyAtk_HitKnockdown4 - EnemyAtk_HitKnockdown3 with a 4.0 push.
 * In:  as EnemyAtk_HitTouchSpark
 * Out: as EnemyAtk_HitKnockdown3
 * Evidence: move.l #$40000,d2 and the fall into $0106F0; callback long at $26D08, $26D72, $363D6,
 *           $42380, $4349E
 */
void EnemyAtk_HitKnockdown4(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5,
                            uint32_t d6, uint32_t d7, uint32_t a0, uint32_t *a1, HitBox *box, uint32_t a3,
                            uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    (void)d3, (void)d5, (void)d7, (void)a0, (void)a3, (void)a4, (void)a5;
    hit_knockdown(0x40000u, d0, d1, d2, d4, d6, a1, box, *a6);
}
