#include "game.h"

/* Op2_LaunchAlongHeading, Op2_AimAtTracked and the turn ops ($0014F0-$001569). */

/* Tbl_059B22: the sine table, a signed word per even heading (index & $3FE as a byte offset),
   1.0 = $80 after the asr #7; cosine a quarter turn on (+$100 bytes) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;
static const s16 *sine_entry(uint16_t byte_index)
{
    PTR(const s16) e;
    e.v = Tbl_059B22 + byte_index;
    return e;
}

/* d.w (signed) times a table value: muls.w */
static uint32_t muls(uint32_t d, int16_t m) { return (uint32_t)((int32_t)(int16_t)(uint16_t)d * m); }

/* Op2_LaunchAlongHeading - a2 op: the velocity = the cruise speeds along the target heading (x
 * by cos, y by sin), and the cruise speeds become the target speeds.
 * In:  mot->heading_target, cruise_x / cruise_y
 * Out: mot->target_x / target_y = the cruise speeds; vel_x = (cruise_x >> 8) * cos >> 7, vel_y the
 *      same with sin; index = the table byte offset (heading & $3FE); d1/d2 = the velocity; NEXT
 * Evidence: lea Tbl_059B22,a0 / move.w $A(a3),d0 / andi.w #$3FE / movem cruise / asr.l #8 /
 *           muls.w $100(a0),d1 / muls.w (a0),d2 / asr.l #7 / movem.l d1-d2,$18(a3); 332 TAS frames
 */
void Op2_LaunchAlongHeading(uint16_t *index, uint32_t *d1, uint32_t *d2, const u16 **script, Vm2Motion *mot)
{
    *index = (uint16_t)(mot->heading_target & 0x3FE);
    const s16 *e = sine_entry(*index);
    *d1 = (uint32_t)mot->cruise_x;
    *d2 = (uint32_t)mot->cruise_y;
    mot->target_x = (int32_t)*d1;
    mot->target_y = (int32_t)*d2;
    *d1 = (uint32_t)((int32_t)muls((uint32_t)((int32_t)*d1 >> 8), e[0x80]) >> 7);
    *d2 = (uint32_t)((int32_t)muls((uint32_t)((int32_t)*d2 >> 8), e[0]) >> 7);
    mot->vel_x = (int32_t)*d1;
    mot->vel_y = (int32_t)*d2;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_AimAtTracked - a2 op: points the target heading at the tracked position (the targeted
 * player record's x/y, handle at a4 +$10); nothing when no record is tracked.
 * In:  vm->tracked; the record's x / y; self's position
 * Out: mot->heading_target; d0/d1 = dx/dy then as Mot_Atan2 leaves them, d2 = the heading; NEXT
 *      (VM2_Next with d0's low word 0 when none)
 * Evidence: move.w $10(a4),d0 / beq.w VM2_Next / movea.w d0,a0 / move.w $2E(a0),d0 / sub.w $3E(a6) /
 *           move.w $32(a0),d1 / sub.w $42(a6) / jsr Mot_Atan2 / andi.w #$3FF,d2 / move.w d2,$A(a3)
 */
void Op2_AimAtTracked(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *handler, const u16 **script,
                      Vm2Motion *mot, const Vm2Block *vm, const Vm2Obj *self)
{
    int16_t h = vm->tracked;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)h;
    if (h == 0) {
        dk::jump(at_VM2_Next);
        return;
    }
    const PlayerRec *rec = dk::handle<const PlayerRec>(h);
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(rec->x - self->x);
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)(rec->y - self->y);
    Mot_Atan2(d0, d1, d2);
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)(*d2 & 0x3FF);
    mot->heading_target = (uint16_t)*d2;
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_SnapHeadingSteer - a2 op: the current heading takes the target heading at once (its
 * integer part; the fraction stays), then the velocity steers along it (Mot_SteerToHeading).
 * In:  mot->heading_target
 * Out: mot->heading; the registers on to Mot_SteerToHeading, jumped to
 * Evidence: move.w $A(a3),$C(a3) / bra.s Mot_SteerToHeading; executed in the TAS
 */
void Op2_SnapHeadingSteer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t *a0, uint32_t *a2, Vm2Motion *mot)
{
    mot->heading = (int32_t)((uint32_t)mot->heading_target << 16 | ((uint32_t)mot->heading & 0xFFFF));
    dk::jump(at_Mot_SteerToHeading);
}

/* Op2_TurnSteer - a2 op: turns the current heading toward the target by the turn rate
 * (Mot_TurnTowardHeading), then steers the velocity along it.
 * In:  mot's headings and turn rate
 * Out: mot->heading; d0-d2 as the turn leaves them, on to Mot_SteerToHeading, jumped to
 * Evidence: bsr.w Mot_TurnTowardHeading / bra.s Mot_SteerToHeading; executed in the TAS
 */
void Op2_TurnSteer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                   uint32_t *d6, uint32_t *a0, uint32_t *a2, Vm2Motion *mot)
{
    Mot_TurnTowardHeading(d0, d1, d2, mot);
    dk::jump(at_Mot_SteerToHeading);
}

/* Op2_TurnHeading - a2 op: turns the current heading toward the target by the turn rate, the
 * velocity untouched.
 * In:  mot's headings and turn rate
 * Out: mot->heading; d0-d2 as Mot_TurnTowardHeading leaves them; NEXT
 * Evidence: bsr.w Mot_TurnTowardHeading / NEXT; executed in the TAS
 */
void Op2_TurnHeading(uint32_t *d0, uint32_t *d1, uint32_t *d2, const u16 **script, Vm2Motion *mot)
{
    Mot_TurnTowardHeading(d0, d1, d2, mot);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
