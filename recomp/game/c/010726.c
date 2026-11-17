#include "game.h"

/* EnemyAtk_HitScriptSpark - a4 native hit: when the struck box can be hit, adds the script's damage
 * word as a flinch (hit type 6) and puts the script's spark at the contact point; else skips both
 * operands.
 * In:  box = the struck box (a2); a4 = the script: word damage, long spark (an a4 script); a6 = the
 *      attacker, d4-d7 = the attack rectangle, a3/a5 (for Hitbox_SpawnAtContact)
 * Out: a4 past the 6 operand bytes; after a hit box +$20 += the damage, +$1A = 6, a1 = the spark and
 *      Hitbox_SpawnAtContact's results (bra.w: its condition codes); else Hitbox_CanHitTarget's d0,
 *      d1, a1 and condition codes (addq.w #6,a4 sets none)
 * Evidence: bsr Hitbox_CanHitTarget / bne / move.w (a4)+,d0 / add.w d0,$20(a2) / move.w #6,$1A(a2) /
 *           movea.l (a4)+,a1 / bra.w Hitbox_SpawnAtContact / else addq.w #6,a4 / rts; Op4_CallNative's
 *           dc.l at $01ED0E (dc.w $20, dc.l Scr4_010964) in the claw fight's falling debris (a4 script
 *           $01ECF2); the native runner stopped at it when the debris touched the player (crane-crash
 *           2026-10-30); 010726.syn checks it
 */
void EnemyAtk_HitScriptSpark(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5,
                             uint32_t d6, uint32_t d7, uint32_t a0, uint32_t *a1, HitBox *box, uint32_t a3,
                             uint32_t *a4, uint32_t a5, uint32_t *a6, dk::Flags *ccr)
{
    (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a0, (void)a3, (void)a5, (void)a6;
    uint16_t owner_id = (uint16_t)*d1;
    Hitbox_CanHitTarget(d0, &owner_id, a1, box, ccr);
    *d1 = (*d1 & 0xFFFF0000u) | owner_id;
    if (!ccr->z) {
        *a4 += 6;   /* addq.w #6,a4: past the damage and the spark */
        return;
    }
    PTR(const u16) damage;
    damage.v = *a4;
    *d0 = (*d0 & 0xFFFF0000u) | *damage;
    *a4 += 2;
    box->damage += (uint16_t)*d0;
    box->hit_type = 6;
    PTR(const u32) spark;
    spark.v = *a4;
    *a1 = *spark;
    *a4 += 4;
    dk::jump(at_Hitbox_SpawnAtContact);
}
