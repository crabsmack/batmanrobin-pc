#include "game.h"

static constexpr uint16_t CLAW_FINGER_SPIN = 8;   /* the finger frame turns 8 units (of $400) a frame */
/* the three fingers: 120 degrees apart ($156 + $156 + $154 = $400, one turn) */
static constexpr uint16_t CLAW_FINGER_STEP[3] = { 0x156, 0x156, 0x154 };

/* Boss_Claw_PlaceFingers - spins the claw's finger frame (g_Rot3DInner angle_a += 8 a frame) and
 * places the three fingers 120 degrees apart ($156, $156, $154) into their point lists
 * In:  claw = a6 (the three finger point lists)
 * Out: the finger point lists filled by Boss_Claw_PlaceFinger (angle_a back where the spin left it
 *      plus a full turn); d0-d7/a1 as the last Boss_Claw_PlaceFinger left them; a2-a6 kept
 * Evidence: movem.l a2-a6,-(a7) / lea g_Rot3DInner,a0 / addq.w #8,(a0) / 3 x (movea.w
 *           $10/$12/$14(a6),a5 / addi.w #$156/$156/$154,(a0) / bsr.w Boss_Claw_PlaceFinger) /
 *           movem.l (a7)+,a2-a6; callers Boss_Claw_SwingSideways/SwingAtCrane/SwingFingers; 1301
 *           TAS frames f10512-f12924
 */
void Boss_Claw_PlaceFingers(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                            uint32_t *d6, uint32_t *d7, uint32_t *a1, const ClawObj *claw)
{
    Rot3D *frame = (Rot3D *)g_Rot3DInner;
    frame->angle_a = (uint16_t)(frame->angle_a + CLAW_FINGER_SPIN);
    for (int i = 0; i < 3; i++) {
        frame->angle_a = (uint16_t)(frame->angle_a + CLAW_FINGER_STEP[i]);
        Boss_Claw_PlaceFinger(d0, d1, d2, d3, d4, d5, d6, d7, frame, a1,
                              (uint32_t)(int32_t)claw->finger[i]);   /* movea.w: sign-extended */
    }
}
