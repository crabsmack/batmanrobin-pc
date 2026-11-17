#include "game.h"

/* The player hit tests and the player attacks' hit callbacks ($0107E8-$010889). */

static constexpr uint32_t Scr4_0108F4 = 0x0108F4u;   /* the short-pause hit spark */
static constexpr uint32_t Scr4_0108CE = 0x0108CEu;   /* the long-pause hit spark */

/* the sprite attribute bit for a mirrored (facing left) object: bit 3 of the attribute's high byte */
static constexpr uint16_t ATTR_FLIP_H = 0x0800;

/* Hitbox_CanHitPlayer - hit test for attacks on players: the struck box must belong to owner id 1
 * or 2 (a player), then Hitbox_CanHitOwner's checks.
 * In:  box = the struck box (a2)
 * Out: d0 low word = the box's id, or 0 (Z set) when the hit is accepted; the flags of the last
 *      test (callers beq on them); d1/a1 as Hitbox_CanHitOwner
 * Evidence: move.w $10(a2),d0 / cmpi.w #1,d0 / beq / cmpi.w #2,d0 / beq / rts; called by the enemy
 *           hit callbacks EnemyAtk_HitPlayerA/B; 154 corpus runs
 */
void Hitbox_CanHitPlayer(uint32_t *d0, uint16_t *d1, uint32_t *a1, HitBox *box, dk::Flags *ccr)
{
    uint16_t id = box->id;
    *d0 = (*d0 & 0xFFFF0000u) | id;
    *ccr = dk::Flags::compare(id, 1, 2, ccr->x);
    if (!ccr->z)
        *ccr = dk::Flags::compare(id, 2, 2, ccr->x);
    if (ccr->z) {
        Hitbox_CanHitOwner(d0, d1, a1, box, ccr);
        return;
    }
}

/* Hitbox_CanHitOwner - the owner half of the enemy-attack hit tests: accepts the hit when the
 * box's owner is not invulnerable (its sprite block's frames counter is 0) and the box was not
 * struck yet this frame; marks it struck.
 * In:  box = the struck box (a2)
 * Out: d0 = 0 and Z set when accepted, else d0 unchanged and Z clear; d1 = the owner's id ($58);
 *      a1 = the owner's sprite block
 * Evidence: movea.w $2E(a2),a1 / move.w $58(a1),d1 / movea.w $C(a1),a1 / tst.b $16(a1) / bne /
 *           bset #0,$3C(a2) / bne / moveq #0,d0; branched to by Hitbox_CanHitPlayer and
 *           Hitbox_CanHitTarget; the player object sets $16 = $78 on a spawn
 */
void Hitbox_CanHitOwner(uint32_t *d0, uint16_t *d1, uint32_t *a1, HitBox *box, dk::Flags *ccr)
{
    const Vm2Obj *owner = dk::handle<const Vm2Obj>(box->owner);
    *d1 = owner->owner_id;
    const Vm2Sprite *spr = dk::handle<const Vm2Sprite>(owner->sprite);
    *a1 = dk::addr(spr);
    *ccr = dk::Flags::test(spr->invulnerable, 1, ccr->x);
    if (!ccr->z)
        return;
    bool was_struck = (box->flags & BOX_STRUCK) != 0;
    box->flags = (uint8_t)(box->flags | BOX_STRUCK);
    ccr->z = !was_struck;   /* bset sets Z from the old bit; N, V, C stay tst.b's */
    if (was_struck)
        return;
    *d0 = 0;
    *ccr = dk::Flags::test(0, 4, ccr->x);
}

/* The common tail of the player hit callbacks (PlayerAtk_ApplyHit, $010858, same file): marks the
 * box struck, moves the attack's damage from the box's energy to its damage taken, gives it the
 * knockback (+$36 x 256, negated when the attacker faces left), pushes the attacker back 1.0 the
 * other way unless its contact bit 3 is set, then falls into Hitbox_SpawnAtContact (a jump: the
 * callbacks' prototypes hand on a1, the spark). */
static void apply_player_hit(uint32_t *d0, uint32_t *d1, const HitBox *attack, HitBox *box, Vm2Motion *mot,
                             const Vm2Sprite *spr)
{
    box->flags = (uint8_t)(box->flags | BOX_STRUCK);
    uint16_t damage = attack->hit_damage;
    box->damage += damage;
    box->energy -= damage;
    int32_t knockback = (int32_t)((uint32_t)(int32_t)(int16_t)attack->hit_knockback << 8);
    int32_t push = -0x10000;
    if (spr->attr & ATTR_FLIP_H) {
        knockback = -knockback;
        push = -push;
    }
    box->knockback = knockback;
    uint16_t contact_bit3 = mot->contact & 8;
    *d0 = ((uint32_t)knockback & 0xFFFF0000u) | contact_bit3;
    *d1 = (uint32_t)push;
    if (!contact_bit3)
        mot->impulse_x = push;
    dk::jump(at_Hitbox_SpawnAtContact);
}

/* PlayerAtk_HitPause - player attack hit callback: sound $A, the attacker's script waits 8 runs
 * and its movement 6, then the common tail with the short-pause spark Scr4_0108F4.
 * In:  attack = the attack box (a0: +$34 damage, +$36 knockback); box = the struck box (a2);
 *      mot/a4/spr = the attacker's motion, script and sprite blocks (a3/a4/a5); d4-d7 = the
 *      attack rectangle
 * Out: a1 = the spark; then Hitbox_SpawnAtContact's
 * Evidence: move.w #$A,d0 / jsr Snd_PlaySfx / move.b #8,$1(a4) / move.b #6,$1(a3) / lea
 *           Scr4_0108F4,a1 / bra PlayerAtk_ApplyHit; box callback long in the Batman ($4B824,
 *           $4B886) and Robin ($4E360, $4E3C2) scripts
 */
void PlayerAtk_HitPause(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5,
                        uint32_t d6, uint32_t d7, const HitBox *attack, uint32_t *a1, HitBox *box, Vm2Motion *mot,
                        uint32_t *a4, const Vm2Sprite *spr, uint32_t *a6)
{
    (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a6;
    *d0 = (*d0 & 0xFFFF0000u) | 0xA;
    Snd_PlaySfx(d0);
    PTR(Vm2Block) vm;
    vm.v = *a4;
    vm->delay = 8;
    mot->stun = 6;
    *a1 = Scr4_0108F4;
    apply_player_hit(d0, d1, attack, box, mot, spr);
}

/* PlayerAtk_HitLongPause - player attack hit callback: sound $A, the attacker's script waits 12
 * runs (its movement not paused), then the common tail with the spark Scr4_0108CE.
 * In:  as PlayerAtk_HitPause
 * Out: as PlayerAtk_HitPause
 * Evidence: move.w #$A,d0 / jsr Snd_PlaySfx / move.b #$C,$1(a4) / lea Scr4_0108CE,a1 and the fall
 *           into PlayerAtk_ApplyHit; box callback in 5 Batman and 5 Robin attack definitions
 */
void PlayerAtk_HitLongPause(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5,
                            uint32_t d6, uint32_t d7, const HitBox *attack, uint32_t *a1, HitBox *box,
                            Vm2Motion *mot, uint32_t *a4, const Vm2Sprite *spr, uint32_t *a6)
{
    (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a6;
    *d0 = (*d0 & 0xFFFF0000u) | 0xA;
    Snd_PlaySfx(d0);
    PTR(Vm2Block) vm;
    vm.v = *a4;
    vm->delay = 0xC;
    *a1 = Scr4_0108CE;
    apply_player_hit(d0, d1, attack, box, mot, spr);
}
