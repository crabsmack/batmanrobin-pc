#include "game.h"

/* Shot_DamageTarget - native hit op of the weapon-shot a4 scripts: marks the struck box hit by
 * the shot's owner, gives it a 1.0 knockback (leftward when the shot heads left: heading
 * $100-$300), adds the shot's damage and takes it from the box's energy, then branches the script:
 * the first offset when the energy ran out (<= 0), the second otherwise. A box of owner id $B
 * always takes the second offset (whatever its energy) and the shot gets high sprite priority.
 * In:  box = the struck box (a2); script = the script after the native's long (a4: two branch
 *      offset words); self = the shot (a6)
 * Out: d0 low word = the damage; script = the chosen offset's target; the flags of the energy's
 *      sub.w
 * Evidence: bset #0,$3C(a2) / move.w $10(a6),$1A(a2) / move.l #$10000,$22(a2) / andi.w #$3FE /
 *           cmpi.w #$300 / bgt / cmpi.w #$100 / blt / neg.l / add.w d0,$20(a2) / cmpi.w #$B,$10(a2)
 *           / ori.w #$8000,$22(a6) / sub.w d0,$1E(a2) / ble / addq.w #2,a4 / adda.w (a4),a4; the a4
 *           native op $5438 at $505D0, $5090C, $50C46 (the shot scripts of Tbl_010AF8)
 */
void Shot_DamageTarget(uint16_t *d0, HitBox *box, const s16 **script, Vm4Obj *self, dk::Flags *ccr)
{
    box->flags = (uint8_t)(box->flags | BOX_STRUCK);
    box->struck_by = self->owner_id;
    box->knockback = 0x10000;
    uint16_t heading = (uint16_t)self->heading & 0x3FE;
    if ((int16_t)heading <= 0x300 && (int16_t)heading >= 0x100)
        box->knockback = -(int32_t)box->knockback;
    uint16_t damage = self->damage;
    *d0 = damage;
    box->damage += damage;
    bool always_second = box->id == 0xB;
    if (always_second)
        self->attr |= 0x8000;
    uint16_t energy = (uint16_t)box->energy;
    *ccr = dk::Flags::compare(energy, damage, 2);
    ccr->x = ccr->c;   /* sub sets X as C */
    box->energy = (int16_t)(uint16_t)(energy - damage);
    const s16 *p = *script;
    bool ran_out = ccr->z || (ccr->n != ccr->v);   /* ble */
    if (always_second || !ran_out)
        p++;
    *script = (const s16 *)((const u8 *)p + (int16_t)*p);
}

/* Fx_SpawnSkidEffectTimed - native op: the skid effect every 4th frame (g_FrameCounter & 3 = 0),
 * with no motion test; other frames skip the 3 operand words.
 * In:  script = the operands (a2: dx, dy, jitter range); the rest as Fx_SpawnSkidAtArgs
 * Out: d1 = g_FrameCounter with its low byte & 3; the flags of that andi.b (a skip)
 * Evidence: move.l (g_FrameCounter).w,d1 / andi.b #3,d1 / beq.w Fx_SpawnSkidAtArgs, else the fall
 *           into Fx_SkipSkidArgs; native callback (a2 op $2CBC) at $357DE and $35B4E
 */
void Fx_SpawnSkidEffectTimed(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t *d4, uint32_t *d5,
                             uint32_t d6, uint32_t d7, uint32_t a1, uint32_t *a2, uint32_t a3, uint32_t a4,
                             uint32_t a5, uint32_t a6, dk::Flags *ccr)
{
    (void)d0, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7, (void)a1, (void)a3, (void)a4, (void)a5, (void)a6;
    uint32_t fc = g_FrameCounter;
    *d1 = (fc & 0xFFFFFF00u) | (fc & 3u);
    *ccr = dk::Flags::test(*d1, 1, ccr->x);
    if (ccr->z) {
        dk::jump(at_Fx_SpawnSkidAtArgs);
        return;
    }
    Fx_SkipSkidArgs(a2);
}

/* Fx_SkipSkidArgs - the no-spawn exit of the skid-effect natives: skips their 3 operand words.
 * In:  a2 = the operands
 * Out: a2 += 6
 * Evidence: addq.l #6,a2 / rts; branch target of Fx_SpawnSkidEffect's tests and the fall-through
 *           of Fx_SpawnSkidEffectTimed
 */
void Fx_SkipSkidArgs(uint32_t *a2)
{
    *a2 += 6;
}
