#include "game.h"

enum {
    JET1 = 1 << 0, JET2 = 1 << 1, JET3 = 1 << 2,   /* script variable 3: which jets are on */
};
static constexpr uint32_t JET_SPACING = 0x155u << 16;   /* 120 degrees ($400 a turn), the heading's integer word */

/* Enemy_GasOrb_RunJets - runs the gas orb's three gas jets 120 degrees apart: jet 1/2/3 = script
 * variable 3 bit 1/2/4, their puff blocks in the orb's +$12/$14/$16, the jets' heading turned by
 * $155 between them and restored after
 * In:  a3 = the jets' motion block ($E(a6): heading +$C, puff accel/speed), a4 = the VM block (var
 *      3), a5 = the sprite block, a6 = the orb; a2 kept (pushed around the jets)
 * Out: the jets stepped (Enemy_GasOrb_StepJet x 3); d0-d7/a0/a1/a4/a6 as the last jet leaves them
 * Evidence: move.l a2,-(a7) / lea $12(a6),a2 / move.w #1,d0 / bsr.w Enemy_GasOrb_StepJet / addi.w
 *           #$155,$C(a3) / $14, 2 / addi.w / $16, 4 / subi.w #$2AA,$C(a3) / movea.l (a7)+,a2;
 *           Op2_CallNativeRel at $41EAA in the orb's frame loop Scr2_041EA4; TAS 124189-130140
 */
void Enemy_GasOrb_RunJets(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a2, uint32_t a3,
                          uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    PTR(Vm2Motion) jets;
    jets.v = a3;
    PTR(const Vm2Sprite) sprite;
    sprite.v = a5;
    static const uint16_t bits[3] = { JET1, JET2, JET3 };
    uint32_t list = a0;
    for (int i = 0; i < 3; i++) {
        PTR(GasOrbObj) orb;
        orb.v = *a6;                               /* the jet's slot in the orb (StepJet keeps a6) */
        *d0 = (*d0 & 0xFFFF0000u) | bits[i];
        Enemy_GasOrb_StepJet(d0, d1, d2, d3, d4, d5, d6, d7, &list, a1, &orb->jet[i], jets.get(), a4, sprite.get(), a6);
        if (i < 2)
            jets->heading = (int32_t)((uint32_t)jets->heading + JET_SPACING);
    }
    jets->heading = (int32_t)((uint32_t)jets->heading - 2 * JET_SPACING);
}
