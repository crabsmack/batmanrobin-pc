#include "game.h"

static constexpr uint32_t Scr4_010940 = 0x010940u;   /* spark frames $221A-$222E */
static constexpr uint32_t Scr4_01091A = 0x01091Au;   /* spark frames $226A-$227E */

/* EnemyAtk_DamageFoundBox - a2 native: damages the box the last overlap op found (g_FoundBox) by
 * the script's word, as a flinch (hit type 6), when Hitbox_CanHitTarget accepts it.
 * In:  script = the operand (a2: word damage)
 * Out: script past the word; d7 low word = the damage; the flags: Hitbox_CanHitTarget's when no
 *      hit, else those of the move.w #6 (all clear)
 * Evidence: move.w (a2)+,d7 / move.l a2,-(a7) / movea.l (g_FoundBox).w,a2 / bsr Hitbox_CanHitTarget
 *           / bne / add.w d7,$20(a2) / move.w #6,$1A(a2) / movea.l (a7)+,a2; dc.l after op $2CBC
 *           at 12 sites ($1E082, $1F51C ... $284B4)
 */
void EnemyAtk_DamageFoundBox(uint32_t *d0, uint16_t *d1, uint16_t *d7, uint32_t *a1, const u16 **script,
                             dk::Flags *ccr)
{
    *d7 = *(*script)++;
    PTR(HitBox) box;
    box.v = g_FoundBox;
    Hitbox_CanHitTarget(d0, d1, a1, box, ccr);
    if (!ccr->z)
        return;
    box->damage += *d7;
    box->hit_type = 6;
    *ccr = dk::Flags::test(6, 2, ccr->x);
}

/* both players-only callbacks: Hitbox_CanHitPlayer, then (a hit) the spark into a1 and the jump
 * into EnemyAtk_ApplyHit (bra.w: the prototypes hand a1 on) */
static void hit_player(uint32_t spark, uint32_t *d0, uint32_t *d1, uint32_t *a1, HitBox *box)
{
    uint16_t owner_id = (uint16_t)*d1;
    dk::Flags f{};
    Hitbox_CanHitPlayer(d0, &owner_id, a1, box, &f);
    *d1 = (*d1 & 0xFFFF0000u) | owner_id;
    if (!f.z)
        return;
    *a1 = spark;
    dk::jump(at_EnemyAtk_ApplyHit);
}

/* EnemyAtk_HitPlayerA - enemy attack hit callback for players only: Hitbox_CanHitPlayer, then
 * EnemyAtk_ApplyHit with the spark Scr4_010940.
 * In:  as EnemyAtk_ApplyHit (a0 attack box, a2 struck box, a3/a5 attacker blocks, d4-d7)
 * Out: d0 nonzero when no hit (Hitbox_CanHitPlayer's, its a1 too); else a1 = the spark and
 *      EnemyAtk_ApplyHit's
 * Evidence: bsr Hitbox_CanHitPlayer / bne / lea Scr4_010940,a1 / bra.w EnemyAtk_ApplyHit; callback
 *           long at $1BE3A, $251D4, $252AE
 */
void EnemyAtk_HitPlayerA(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5,
                         uint32_t d6, uint32_t d7, const HitBox *attack, uint32_t *a1, HitBox *box, Vm2Motion *mot,
                         uint32_t *a4, const Vm2Sprite *spr, uint32_t *a6)
{
    (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)attack, (void)mot, (void)a4, (void)spr, (void)a6;
    hit_player(Scr4_010940, d0, d1, a1, box);
}

/* EnemyAtk_HitPlayerB - EnemyAtk_HitPlayerA with the spark Scr4_01091A.
 * In:  as EnemyAtk_HitPlayerA
 * Out: as EnemyAtk_HitPlayerA
 * Evidence: bsr Hitbox_CanHitPlayer / bne / lea Scr4_01091A,a1 / bra.w EnemyAtk_ApplyHit; callback
 *           long at $1B4EE, $1BD80, $1BDD4
 */
void EnemyAtk_HitPlayerB(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5,
                         uint32_t d6, uint32_t d7, const HitBox *attack, uint32_t *a1, HitBox *box, Vm2Motion *mot,
                         uint32_t *a4, const Vm2Sprite *spr, uint32_t *a6)
{
    (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)attack, (void)mot, (void)a4, (void)spr, (void)a6;
    hit_player(Scr4_01091A, d0, d1, a1, box);
}
