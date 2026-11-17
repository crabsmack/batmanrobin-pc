#include "game.h"

/* the claw is drawn tilted back this far about its third angle (g_Rot3DOuter angle_c) */
static constexpr uint16_t CLAW_TILT = 0xFFE0;
/* Boss_Claw_SwingAtCraneStep: x = g_CameraX * 0.6 + $3B8: it scrolls with the background crane */
static constexpr uint16_t CLAW_CRANE_PARALLAX_DIV = 0x280;   /* (camera x 16.16 >> 8) / $280 = 0.4 x camera */
static constexpr uint16_t CLAW_CRANE_X = 0x3B8;

/* the chain's 12 points (10 bytes each: x, y, z, size, attr; ROM data, batmanrobin.asm) */
static constexpr uint32_t Tbl_ClawChainPoints = 0x01EC0Eu;
static constexpr uint32_t CLAW_CHAIN_BALLS = 12;
static constexpr uint32_t CLAW_FINGER_BALLS = 3;

static Rot3D *outer(void) { return (Rot3D *)g_Rot3DOuter; }
static Rot3D *inner(void) { return (Rot3D *)g_Rot3DInner; }

/* divu.w: quotient in the low word, remainder in the high word; on overflow the register stays */
static uint32_t divu_w(uint32_t d, uint16_t s)
{
    uint32_t q = d / s, r = d % s;
    if (q > 0xFFFF) return d;
    return (r << 16) | q;
}

/* Boss_Claw_TickHitPause - claw native (switch): while the hit-pause counter runs, counts it down
 * and returns 1 (the script skips its move), else 0
 * In:  a6 = the claw
 * Out: d0 = 1 while paused (moveq), else d0 low word = 0 (the counter's move.w)
 * Evidence: move.w $30(a6),d0 / beq rts / subq.w #1,$30(a6) / moveq #1,d0; op $2D30
 *           (CallNativeRelSwitch) at $1E7A6 in the claw's frame loop; $30 set to 8 by
 *           Boss_Claw_HitCallback; TAS f10512-f12925 (1849 frames)
 */
void Boss_Claw_TickHitPause(uint32_t *d0, ClawObj *claw)
{
    uint16_t pause = claw->hit_pause;
    *d0 = (*d0 & 0xFFFF0000u) | pause;
    if (pause == 0)
        return;
    claw->hit_pause = (uint16_t)(pause - 1);
    *d0 = 1;
}

/* Boss_Claw_SetSwing - claw native: starts a pendulum swing from three script words: angle (<< 16),
 * restoring acceleration, and the claw's depth (g_Rot3DOuter tz's integer part); velocity 0
 * In:  script = the words after the native's call op (a2); claw = a6
 * Out: the swing set; script past the 3 words; d0 = the acceleration (zero-extended)
 * Evidence: moveq #0,d0 / move.w (a2)+,d0 / swap / move.l d0,$1E(a6) / moveq #0 / move.w (a2)+,d0 /
 *           move.l d0,$22(a6) / clr.l $26(a6) / move.w (a2)+,g_Rot3DOuter+$14; op $2CB2 at $1E8C8
 *           ($60, $C000, $100), $1E93E, $1E972 in Scr2_01E87E; TAS only
 */
void Boss_Claw_SetSwing(uint32_t *d0, const u16 **script, ClawObj *claw)
{
    const u16 *w = *script;
    claw->swing_angle = (int32_t)((uint32_t)(uint16_t)w[0] << 16);
    *d0 = (uint16_t)w[1];
    claw->swing_accel = (int32_t)*d0;
    claw->swing_vel = 0;
    outer()->tz = (int32_t)(((uint32_t)(uint16_t)w[2] << 16) | ((uint32_t)outer()->tz & 0xFFFFu));
    *script = w + 3;
}

/* Boss_Claw_StepSwing - steps the claw's pendulum: velocity += acceleration toward angle 0 (negated
 * while the angle is >= 0), angle += velocity; returns the angle
 * In:  claw = a6
 * Out: d0 = the angle swapped (the integer angle in the low word); d1 = the velocity; d2 = the
 *      signed acceleration
 * Evidence: move.l $22(a6),d2 / move.l $26(a6),d1 / move.l $1E(a6),d0 / bmi / neg.l d2 / add.l d2,d1 /
 *           add.l d1,d0 / move.l d0,$1E(a6) / move.l d1,$26(a6) / swap d0; bsr from
 *           Boss_Claw_SwingSideways/SwingAtCraneStep/SwingFingers; no damping
 */
void Boss_Claw_StepSwing(uint32_t *d0, uint32_t *d1, uint32_t *d2, ClawObj *claw)
{
    *d2 = (uint32_t)(int32_t)claw->swing_accel;
    *d1 = (uint32_t)(int32_t)claw->swing_vel;
    uint32_t angle = (uint32_t)(int32_t)claw->swing_angle;
    if ((int32_t)angle >= 0)
        *d2 = 0u - *d2;
    *d1 += *d2;
    angle += *d1;
    claw->swing_angle = (int32_t)angle;
    claw->swing_vel = (int32_t)*d1;
    *d0 = (angle << 16) | (angle >> 16);
}

/* Boss_Claw_SwingSideways - claw slot-2 native: swings the whole claw about the third angle
 * (g_Rot3DOuter angle_c = swing - $20, the other angles 0, the fingers' tilt 0), then places the
 * fingers
 * In:  claw = a6
 * Out: the angles; then as Boss_Claw_PlaceFingers (d0 low word = the tilted angle)
 * Evidence: bsr.s Boss_Claw_StepSwing / lea g_Rot3DOuter,a0 / addi.w #$FFE0,d0 / move.w d0,8(a0) /
 *           clr.w 4(a0) / clr.w (a0) / lea g_Rot3DInner,a0 / clr.w 4(a0) / clr.w 8(a0) / bra.w
 *           Boss_Claw_PlaceFingers; installed by Op2_StartCoroutine2Rel at $1E97A; 3 corpus runs
 */
void Boss_Claw_SwingSideways(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                             uint32_t *d6, uint32_t *d7, uint32_t *a1, ClawObj *claw)
{
    Boss_Claw_StepSwing(d0, d1, d2, claw);
    uint16_t angle = (uint16_t)(*d0 + CLAW_TILT);
    *d0 = (*d0 & 0xFFFF0000u) | angle;
    outer()->angle_c = angle;
    outer()->angle_b = 0;
    outer()->angle_a = 0;
    inner()->angle_b = 0;
    inner()->angle_c = 0;
    dk::jump(at_Boss_Claw_PlaceFingers);
}

/* Boss_Claw_SwingAtCrane - claw slot-2 native: places the fingers once, then makes
 * Boss_Claw_SwingAtCraneStep the slot-2 routine (the claw hanging at the background crane) and runs it
 * In:  vm = the claw's VM block (a4); claw = a6
 * Out: vm coroutine 2 = Boss_Claw_SwingAtCraneStep; then as it
 * Evidence: bsr.w Boss_Claw_PlaceFingers / move.l #Boss_Claw_SwingAtCraneStep,$40(a4) and the fall
 *           into it; installed by Op2_StartCoroutine2Rel at $1E946; TAS only; shot f11460
 */
void Boss_Claw_SwingAtCrane(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a1, Vm2Block *vm, ClawObj *claw)
{
    Boss_Claw_PlaceFingers(d0, d1, d2, d3, d4, d5, d6, d7, a1, claw);
    vm->coroutine[2].v = at_Boss_Claw_SwingAtCraneStep;
    Boss_Claw_SwingAtCraneStep(d0, d1, d2, claw);
}

/* Boss_Claw_SwingAtCraneStep - per-frame part of Boss_Claw_SwingAtCrane: keeps the claw at x =
 * g_CameraX * 0.6 + $3B8 (it scrolls with the background crane) and swings it about the second
 * angle (g_Rot3DOuter angle_b, tilt angle_c = -$20)
 * In:  claw = a6
 * Out: the claw's x; the angles; d0-d2 as Boss_Claw_StepSwing
 * Evidence: move.l g_CameraX,d0 / lsr.l #8 / divu.w #$280 / neg.w / addi.w #$3B8 / add.w g_CameraX,d0 /
 *           move.w d0,$3E(a6) / bsr.s Boss_Claw_StepSwing / angle_b = d0, angle_c = $FFE0, angle_a
 *           = 0; inner angle_b/c = 0; computed entry at $1EA24 in 3 corpus runs (slot 2, called by
 *           Op2_CallSlot2Native)
 */
void Boss_Claw_SwingAtCraneStep(uint32_t *d0, uint32_t *d1, uint32_t *d2, ClawObj *claw)
{
    uint32_t q = divu_w(g_CameraX >> 8, CLAW_CRANE_PARALLAX_DIV);
    uint16_t x = (uint16_t)(CLAW_CRANE_X - (uint16_t)q + (uint16_t)(g_CameraX >> 16));
    claw->x = (int16_t)x;
    Boss_Claw_StepSwing(d0, d1, d2, claw);
    outer()->angle_b = (uint16_t)*d0;
    outer()->angle_c = CLAW_TILT;
    outer()->angle_a = 0;
    inner()->angle_b = 0;
    inner()->angle_c = 0;
}

/* Boss_Claw_SwingFingers - claw slot-2 native: swings the fingers' frame about its second angle
 * (g_Rot3DInner angle_b) with the claw itself fixed at tilt -$20, then places the fingers
 * In:  claw = a6
 * Out: the angles; then as Boss_Claw_PlaceFingers
 * Evidence: bsr.w Boss_Claw_StepSwing / inner angle_b = d0, angle_c = 0 / outer angle_c = $FFE0,
 *           angle_b = 0, angle_a = 0 / bra.w Boss_Claw_PlaceFingers; installed by
 *           Op2_StartCoroutine2Rel at $1E8D0; TAS only
 */
void Boss_Claw_SwingFingers(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a1, ClawObj *claw)
{
    Boss_Claw_StepSwing(d0, d1, d2, claw);
    inner()->angle_b = (uint16_t)*d0;
    inner()->angle_c = 0;
    outer()->angle_c = CLAW_TILT;
    outer()->angle_b = 0;
    outer()->angle_a = 0;
    dk::jump(at_Boss_Claw_PlaceFingers);
}

/* Boss_Claw_Draw - claw native (switch): builds the claw's 3D matrix, draws its 12-ball chain and its
 * three 3-segment fingers, and returns whether the claw is at the players' depth (its attack box may
 * hit)
 * In:  claw = a6 (the chain's sprite slots and the three finger point lists)
 * Out: d0 low word = the claw's hittable flag; d1-d7/a1 as the drawing left them; a2-a6 kept
 * Evidence: movem.l a2-a6,-(a7) / Rot3D_BuildMatrix g_Rot3DOuter / movea.w $E(a6),a5 / lea
 *           Tbl_ClawChainPoints,a1 / moveq #$B,d7 / bsr Boss_Claw_DrawChain / 3 x (movea.w
 *           $10/$12/$14(a6),a1 / moveq #2,d7 / bsr Boss_Claw_DrawPoints) / movem.l (a7)+,a2-a6 /
 *           move.w $2E(a6),d0; op $2D30 at $1E7B6, also op $2CB2 at $1E7C6; TAS f10512-f12925
 */
void Boss_Claw_Draw(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                    uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5,
                    ClawObj *claw)
{
    (void)a5;
    /* movem.l a2-a6: restored at the end, but the drawing runs with the matrix build's a2
       (Boss_Claw_DrawChain / DrawPoints read it) */
    uint32_t r2 = a2, r3 = a3, r4 = a4;
    Rot3D_BuildMatrix(d0, d1, d2, d3, outer(), &r2, &r3, &r4);
    uint32_t a0 = 0;
    uint32_t slots = (uint32_t)(int32_t)claw->chain;   /* movea.w: sign-extended */
    *a1 = Tbl_ClawChainPoints;
    *d7 = CLAW_CHAIN_BALLS - 1;            /* moveq: the whole register */
    uint16_t w6 = (uint16_t)*d6;
    Boss_Claw_DrawChain(d0, d1, d2, d3, d4, d5, &w6, d7, &a0, a1, r2, &slots, claw);
    for (int i = 0; i < 3; i++) {
        *a1 = (uint32_t)(int32_t)claw->finger[i];
        *d7 = CLAW_FINGER_BALLS - 1;
        Boss_Claw_DrawPoints(d0, d1, d2, d3, d4, d5, &w6, d7, a1, r2, &slots, ADDR68(claw));
    }
    *d6 = (*d6 & 0xFFFF0000u) | w6;
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)claw->hittable;
}
