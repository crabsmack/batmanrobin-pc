#include "game.h"

/* Player_ClearBits300 and the knockback hit callbacks of the enemy attacks ($0105AE-$010691). */

static constexpr uint32_t Scr4_010964 = 0x010964u;   /* the knockback hit spark */

/* Player_ClearBits300 - clears bits $300 of one in-play player's script variable 1.
 * In:  rec = the player record (a0)
 * Out: none
 * Evidence: btst #0,$25(a0) / beq / movea.w $2C(a0),a0 / movea.w $A(a0),a0 / andi.w #$FCFF,$24(a0);
 *           tail of Player_ClearBits300All; TAS frame 33603
 */
void Player_ClearBits300(const PlayerRec *rec)
{
    if (!(rec->flags & PLAYER_IN_PLAY))
        return;
    const Vm2Obj *obj = dk::handle<const Vm2Obj>(rec->object);
    dk::handle<Vm2Block>(obj->vm)->var[1] &= 0xFCFF;
}

/* the knockback of an attack box: its +$36 word x 256 (16.16) */
static int32_t knockback_of(const HitBox *attack)
{
    return (int32_t)((uint32_t)(int32_t)(int16_t)attack->hit_knockback << 8);
}

/* the struck box's centre x on screen (the low word of d2: (left + right) / 2 - the camera's x) */
static uint16_t centre_on_screen(const HitBox *box)
{
    uint16_t sum = (uint16_t)((uint16_t)box->left + (uint16_t)box->right);
    return (uint16_t)((uint16_t)((int16_t)sum >> 1) - (uint16_t)(g_CameraX >> 16));
}

/* the edge cases of both knockback callbacks: within $30 of the left edge the push is as it
 * comes (rightward for a positive knockback), from $110 on it is negated (leftward); 1.25x either
 * way. Returns false in mid-screen. */
static bool edge_push(uint32_t *d0, uint32_t *d1, uint32_t *d2, const HitBox *box)
{
    uint16_t x = centre_on_screen(box);
    *d2 = (*d2 & 0xFFFF0000u) | x;
    /* native: the edges of the players' area widened to the wide view (the gameplay mode, R20) */
    int16_t near_left = (int16_t)(0x30 - WS_AREA_L()), near_right = (int16_t)(0x110 + WS_AREA_R());
    if ((int16_t)x > near_left && (int16_t)x < near_right)
        return false;
    if ((int16_t)x > near_left)
        *d0 = 0u - *d0;
    *d1 = (uint32_t)((int32_t)*d0 >> 2);
    *d0 += *d1;
    return true;
}

/* EnemyAtk_HitKnockSpeed - enemy attack hit callback: when Hitbox_CanHitTarget accepts the box,
 * EnemyAtk_ApplyKnockSpeed.
 * In:  a0 = the attack box (+$34 damage, +$36 knockback); box = the struck box (a2); mot = the
 *      attacker's motion block (a3); d4-d7 = the attack rectangle
 * Out: d0 = 0 after a hit, else Hitbox_CanHitTarget's d0 (nonzero)
 * Evidence: jsr Hitbox_CanHitTarget / beq.s EnemyAtk_ApplyKnockSpeed / rts; box callback long at
 *           52 sites ($18114, $28DC6, $35AD0 ...)
 */
void EnemyAtk_HitKnockSpeed(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a0, HitBox *box, const Vm2Motion *mot, uint32_t *a4,
                            uint32_t a5, uint32_t *a6)
{
    uint16_t owner_id = (uint16_t)*d1;
    uint32_t a1 = 0;
    dk::Flags f{};
    Hitbox_CanHitTarget(d0, &owner_id, &a1, box, &f);
    *d1 = (*d1 & 0xFFFF0000u) | owner_id;
    if (!f.z)
        return;
    EnemyAtk_ApplyKnockSpeed(d0, d1, d2, d3, d4, d5, d6, d7, a0, box, mot, a4, a5, a6);
}

/* EnemyAtk_ApplyKnockSpeed - the knockback of EnemyAtk_HitKnockSpeed after the hit test: near the
 * screen edges pushed inward x1.25; in mid-screen, when the attacker moves right (negated x
 * velocity negative) the push is leftward unless the attacker's speed beats it, and when it moves
 * left or stands the push is as it comes unless the speed beats it; a speed that beats the push
 * leaves the knockback for EnemyAtk_KnockAwayFromBox to point away from the attack box.
 * In:  as EnemyAtk_HitKnockSpeed, the hit already accepted
 * Out: as EnemyAtk_ApplyKnock
 * Evidence: the asm at $0105D0-$010613 (ble/blt on $30/$110 of the centre minus g_CameraX, neg.l
 *           $18(a3) / bpl / cmp.l d0,d2 / blt or bgt); jmp targets from $3B6FA and $3DE62
 */
void EnemyAtk_ApplyKnockSpeed(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a0, HitBox *box, const Vm2Motion *mot,
                              uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    PTR(const HitBox) attack;
    attack.v = *a0;
    *d0 = (uint32_t)knockback_of(attack);
    dk::Flags f{};
    if (edge_push(d0, d1, d2, box)) {
        EnemyAtk_ApplyKnock(d0, d1, d2, d3, d4, d5, d6, d7, a0, box, dk::addr(mot), a4, a5, a6, &f);
        return;
    }
    *d2 = 0u - (uint32_t)(int32_t)mot->vel_x;
    if ((int32_t)*d2 < 0) {
        *d0 = 0u - *d0;
        if ((int32_t)*d2 < (int32_t)*d0) {
            EnemyAtk_ApplyKnock(d0, d1, d2, d3, d4, d5, d6, d7, a0, box, dk::addr(mot), a4, a5, a6, &f);
            return;
        }
        *d0 = 0u - *d0;
    } else if ((int32_t)*d2 > (int32_t)*d0) {
        EnemyAtk_ApplyKnock(d0, d1, d2, d3, d4, d5, d6, d7, a0, box, dk::addr(mot), a4, a5, a6, &f);
        return;
    }
    EnemyAtk_KnockAwayFromBox(d0, d1, d2, d3, d4, d5, d6, d7, a0, box, dk::addr(mot), a4, a5, a6, &f);
}

/* EnemyAtk_HitKnock - enemy attack hit callback like EnemyAtk_HitKnockSpeed without the attacker
 * speed case: near the screen edges pushed inward x1.25, in mid-screen away from the attack box.
 * In:  as EnemyAtk_HitKnockSpeed
 * Out: d0 = 0 and Z set after a hit; else Hitbox_CanHitTarget's d0 and flags (Z clear)
 * Evidence: jsr Hitbox_CanHitTarget / beq.w / rts, then the edge test as $0105D0 and bra
 *           EnemyAtk_KnockAwayFromBox in mid-screen; callback long at $1C54E, $1C562, $25420, jsr
 *           from $1E768
 */
void EnemyAtk_HitKnock(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint32_t *d7, uint32_t *a0, HitBox *box, uint32_t a3, uint32_t *a4, uint32_t a5,
                       uint32_t *a6, dk::Flags *ccr)
{
    uint16_t owner_id = (uint16_t)*d1;
    uint32_t a1 = 0;
    Hitbox_CanHitTarget(d0, &owner_id, &a1, box, ccr);
    *d1 = (*d1 & 0xFFFF0000u) | owner_id;
    if (!ccr->z)
        return;
    PTR(const HitBox) attack;
    attack.v = *a0;
    *d0 = (uint32_t)knockback_of(attack);
    if (edge_push(d0, d1, d2, box)) {
        EnemyAtk_ApplyKnock(d0, d1, d2, d3, d4, d5, d6, d7, a0, box, a3, a4, a5, a6, ccr);
        return;
    }
    EnemyAtk_KnockAwayFromBox(d0, d1, d2, d3, d4, d5, d6, d7, a0, box, a3, a4, a5, a6, ccr);
}

/* EnemyAtk_KnockAwayFromBox - points the knockback away from the attack box: negated when the
 * struck box's centre (doubled: left + right) is left of the attack box's (x1 + x2).
 * In:  d0 = the knockback (16.16); box = the struck box (a2); d4/d6 = the attack box's x1/x2
 * Out: as EnemyAtk_ApplyKnock; d2 low word = the centre difference
 * Evidence: move.w $12(a2),d2 / add.w $16(a2),d2 / sub.w d4,d2 / sub.w d6,d2 / bpl / neg.l d0 and
 *           the fall into EnemyAtk_ApplyKnock
 */
void EnemyAtk_KnockAwayFromBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                               uint32_t *d6, uint32_t *d7, uint32_t *a0, HitBox *box, uint32_t a3, uint32_t *a4, uint32_t a5,
                               uint32_t *a6, dk::Flags *ccr)
{
    uint16_t diff = (uint16_t)((uint16_t)box->left + (uint16_t)box->right - (uint16_t)*d4 - (uint16_t)*d6);
    *d2 = (*d2 & 0xFFFF0000u) | diff;
    if ((int16_t)diff < 0)
        *d0 = 0u - *d0;
    EnemyAtk_ApplyKnock(d0, d1, d2, d3, d4, d5, d6, d7, a0, box, a3, a4, a5, a6, ccr);
}

/* EnemyAtk_ApplyKnock - common tail of the knockback callbacks: the attack's damage, hit type 8
 * (knockback) or 9 (knockdown) when the struck object's motion has none of the terrain contact
 * bits $B (airborne), the knockback d0, and the spark Scr4_010964 at the contact point.
 * In:  d0 = the knockback (16.16); a0 = the attack box; box = the struck box (a2); d4-d7 = the
 *      attack rectangle
 * Out: d0 = 0, Z set; d1/d2 as Hitbox_SpawnAtContact left them
 * Evidence: move.w $34(a0),d1 / add.w d1,$20(a2) / movea.w $2E(a2),a1 / movea.w $8(a1),a1 / move.w
 *           #8,d1 / move.w $54(a1),d2 / andi.w #$B,d2 / bne / moveq #9,d1 / move.w d1,$1A(a2) /
 *           move.l d0,$22(a2) / lea Scr4_010964,a1 / bsr Hitbox_SpawnAtContact / moveq #0,d0
 */
void EnemyAtk_ApplyKnock(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                         uint32_t *d6, uint32_t *d7, uint32_t *a0, HitBox *box, uint32_t a3, uint32_t *a4, uint32_t a5,
                         uint32_t *a6, dk::Flags *ccr)
{
    PTR(const HitBox) attack;
    attack.v = *a0;
    box->damage += attack->hit_damage;
    const Vm2Obj *owner = dk::handle<const Vm2Obj>(box->owner);
    const Vm2Motion *mot = dk::handle<const Vm2Motion>(owner->motion);
    uint16_t contact = mot->contact & 0xB;
    *d1 = (*d1 & 0xFFFF0000u) | 8;
    *d2 = (*d2 & 0xFFFF0000u) | contact;
    if (!contact)
        *d1 = 9;
    box->hit_type = (uint16_t)*d1;
    box->knockback = (int32_t)*d0;
    Hitbox_SpawnAtContact(d0, d1, d2, *d3, *d4, *d5, *d6, *d7, *a0, Scr4_010964, box, a3, a4, a5, a6);
    *d0 = 0;
    *ccr = dk::Flags::test(0, 4, ccr->x);
}
