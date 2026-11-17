#include "game.h"

/* Obj_TestBoxStruck and the energy ops ($00398A-$003A73): Op2_SetEnergy1P2P / Op2_SetEnergy,
 * Op2_BranchEnergyAtPct, the hit-taking ops Op2_TakeDamage / Op2_TakeKnockback / Op2_TakeHit, and
 * the boss meter ops Op2_SetBossEnergy / Op2_AddBossEnergy. */

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the box g_StruckBox holds (an address, as Obj_SetStruckBox stored it) */
static HitBox *struck_box(void)
{
    PTR(HitBox) box;
    box.v = g_StruckBox;
    return box;
}

/* Obj_TestBoxStruck - tests and clears the struck bit of one body box chosen by a word operand (an
 * index at or past the count, or an empty slot, counts as not struck).
 * In:  script = word index; vm->body_count / body[]
 * Out: struck: g_StruckBox = the box, d0 = 1; else d0 = 0 (Z from d0); a1 = the handle list (once
 *      the index is in range); script past the index
 * Evidence: move.w (a2)+,d0 / cmp.w $2(a4),d0 / bge.s / lea $4(a4),a1 / add.w d0,d0 /
 *           move.w (a1,d0.w),d0 / beq.s / movea.w d0,a0 / bclr #0,$3C(a0) / bne.s $3982 /
 *           moveq #0,d0; 7 runs
 */
void Obj_TestBoxStruck(uint32_t *d0, uint32_t *a1, const u16 **script, Vm2Block *vm)
{
    set_low_word(d0, *(*script)++);
    if ((int16_t)(uint16_t)*d0 >= (int16_t)(uint16_t)vm->body_count) {
        *d0 = 0;
        return;
    }
    const s16 *list = &vm->body[0];
    *a1 = dk::addr(list);
    set_low_word(d0, (uint16_t)(*d0 + *d0));
    set_low_word(d0, (uint16_t)*(const s16 *)((const u8 *)list + (int16_t)(uint16_t)*d0));
    if ((uint16_t)*d0 == 0) {
        *d0 = 0;
        return;
    }
    HitBox *box = dk::handle<HitBox>((int16_t)(uint16_t)*d0);
    bool struck = box->flags & BOX_STRUCK;
    box->flags = (uint8_t)(box->flags & ~BOX_STRUCK);
    if (struck) {
        /* bne.s Obj_SetStruckBox: its two instructions (the box is in a0, which this function's
           convention doesn't hand on) */
        g_StruckBox = dk::addr(box);
        *d0 = 1;
        return;
    }
    *d0 = 0;
}

/* the shared scaling ($39BC): energy = given * scale >> 8 (the boss scale for owner class $B), and
   1% of it; d0 as the asm leaves it (divu: remainder high, quotient low) */
static void set_energy(uint32_t *d0, uint16_t given, const u16 **script, Vm2Obj *self)
{
    uint16_t scale = g_EnemyEnergyScale;
    if (self->owner_id == 0xB)
        scale = g_BossEnergyScale;
    uint32_t v = ((uint32_t)scale * given) >> 8;
    self->energy = (int16_t)(uint16_t)v;
    v &= 0xFFFFu;
    uint16_t pct = (uint16_t)(v / 100u);
    *d0 = ((v % 100u) << 16) | pct;
    self->energy_pct = pct;
    uint32_t handler;
    next(&handler, script);
}

/* Op2_SetEnergy1P2P - a2 op: sets the object's energy from one of two operands, the second when
 * two players are in play, then scales it as Op2_SetEnergy.
 * In:  script = word energy (1 player), word energy (2 players)
 * Out: energy (d1's low word) = the operand used; self->energy / energy_pct; d0 = the division's;
 *      NEXT
 * Evidence: move.w (a2)+,d1 / cmpi.w #2,(g_PlayersInPlay).w / beq.s Op2_SetEnergy / addq.l #2,a2 /
 *           bra.s $39BC; 42 runs; $1809E `39AC 2200 3200`
 */
void Op2_SetEnergy1P2P(uint32_t *d0, uint16_t *energy, const u16 **script, Vm2Obj *self)
{
    *energy = *(*script)++;
    if (g_PlayersInPlay == 2) {
        Op2_SetEnergy(d0, energy, script, self);
        return;
    }
    (*script)++;
    set_energy(d0, *energy, script, self);
}

/* Op2_SetEnergy - a2 op: sets the object's energy to operand * g_EnemyEnergyScale >> 8
 * (g_BossEnergyScale for owner class $B) and its 1% unit to energy / 100.
 * In:  script = word energy
 * Out: energy (d1's low word) = the operand; self->energy / energy_pct; d0 = the division's; NEXT
 * Evidence: move.w (a2)+,d1 / mulu.w d1,d0 / lsr.l #8,d0 / move.w d0,$52(a6) / andi.l #$FFFF,d0 /
 *           divu.w #$64,d0 / move.w d0,$54(a6); dispatched at $39BA in 125 runs; $1ACF4 `39BA 00D0`
 */
void Op2_SetEnergy(uint32_t *d0, uint16_t *energy, const u16 **script, Vm2Obj *self)
{
    *energy = *(*script)++;
    set_energy(d0, *energy, script, self);
}

/* Op2_BranchEnergyAtPct - a2 op: branches when the object's energy is at or below N% of the energy
 * it was given (N * the 1% unit).
 * In:  script = word percent N, word offset
 * Out: d0 = N * energy_pct; at or below: Op2_Goto, else NEXT after the offset
 * Evidence: move.w $54(a6),d0 / mulu.w (a2)+,d0 / cmp.w $52(a6),d0 / bge.w Op2_Goto; 11 runs;
 *           $3E034 `39E6 000A 0004` (10%)
 */
void Op2_BranchEnergyAtPct(uint32_t *d0, uint32_t *a0, const u16 **script, const Vm2Obj *self)
{
    *d0 = (uint32_t)self->energy_pct * *(*script)++;
    if ((int16_t)(uint16_t)*d0 >= self->energy) {
        dk::jump(at_Op2_Goto);
        return;
    }
    (*script)++;
    next(a0, script);
}

/* the damage half of Op2_TakeDamage / Op2_TakeHit: the struck box's damage (then cleared) off the
   energy; above 0: Op2_SkipWord (no branch), else the energy = 0 and the branch */
static void take_damage(uint16_t *damage, uint32_t *a0, HitBox *box, const u16 **script, Vm2Obj *self)
{
    *damage = box->damage;
    box->damage = 0;
    /* sub.w / bgt: the exact difference above 0 */
    int32_t left = (int32_t)self->energy - (int16_t)*damage;
    self->energy = (int16_t)left;
    if (left > 0) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    self->energy = 0;
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(a0, script);
}

/* Op2_TakeDamage - a2 op: subtracts the struck box's damage (then cleared) from the object's
 * energy; branches when the energy runs out (clamped to 0).
 * In:  script = word offset; g_StruckBox (from Op2_BranchStruck)
 * Out: damage (d0's low word); a0 = the box; energy left: Op2_SkipWord (NEXT after the offset),
 *      else self->energy = 0 and the branch
 * Evidence: movea.l (g_StruckBox).w,a0 / move.w $20(a0),d0 / clr.w $20(a0) / sub.w d0,$52(a6) /
 *           bgt.w Op2_SkipWord / clr.w $52(a6) / adda.w (a2),a2 / NEXT; 97 runs
 */
void Op2_TakeDamage(uint16_t *damage, uint32_t *a0, const u16 **script, Vm2Obj *self)
{
    *a0 = g_StruckBox;
    take_damage(damage, a0, struck_box(), script, self);
}

/* Op2_TakeKnockback - a2 op: applies the struck box's knockback (the attack's push, signed by the
 * attacker's facing) as the motion block's x impulse.
 * In:  g_StruckBox
 * Out: mot->impulse_x = box->knockback; NEXT
 * Evidence: movea.l (g_StruckBox).w,a0 / move.l $22(a0),$2(a3) / NEXT; 20 runs
 */
void Op2_TakeKnockback(const u16 **script, Vm2Motion *mot)
{
    mot->impulse_x = struck_box()->knockback;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_TakeHit - a2 op: Op2_TakeKnockback and Op2_TakeDamage in one.
 * In:  script = word offset; g_StruckBox
 * Out: mot->impulse_x; then as Op2_TakeDamage
 * Evidence: move.l $22(a0),$2(a3) then the Op2_TakeDamage body; 80 runs
 */
void Op2_TakeHit(uint16_t *damage, uint32_t *a0, const u16 **script, Vm2Motion *mot, Vm2Obj *self)
{
    *a0 = g_StruckBox;
    HitBox *box = struck_box();
    mot->impulse_x = box->knockback;
    take_damage(damage, a0, box, script, self);
}

/* Op2_SetBossEnergy - a2 op: shows the object's energy on the boss meter.
 * In:  self->energy
 * Out: g_BossEnergy; handler = the next op's; NEXT
 * Evidence: move.w $52(a6),(g_BossEnergy).w / NEXT; 38 runs
 */
void Op2_SetBossEnergy(uint32_t *a0, const u16 **script, const Vm2Obj *self)
{
    g_BossEnergy = (uint16_t)self->energy;
    next(a0, script);
}

/* Op2_AddBossEnergy - a2 op: adds the object's energy to the boss meter's (bosses made of parts).
 * In:  self->energy
 * Out: g_BossEnergy += it; handler = the next op's; NEXT
 * Evidence: move.w $52(a6),d0 / add.w d0,(g_BossEnergy).w / NEXT; 10 runs
 */
void Op2_AddBossEnergy(uint32_t *a0, const u16 **script, const Vm2Obj *self)
{
    g_BossEnergy = (uint16_t)(g_BossEnergy + (uint16_t)self->energy);
    next(a0, script);
}
