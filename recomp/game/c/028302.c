#include "game.h"

/* Enemy_Biplane_RandAnimPhase - red biplane native: starts its propeller animation counter
 * (sprite var 0) at a random value so the planes of a wave don't flap in step
 * In:  sprite = the plane's sprite block (a5)
 * Out: d0 = Rand_Next's value; sprite var 0 = its low word
 * Evidence: jsr Rand_Next / move.w d0,$22(a5) / rts; op $2CB2 (CallNativeRel) at $0282B0 at the
 *           start of the biplane body $028286 (anim Scr2_028326: a small red propeller plane);
 *           TAS f36400-f66213 (108 frames, one per plane)
 */
uint32_t Enemy_Biplane_RandAnimPhase(Vm2Sprite *sprite)
{
    uint32_t r = (uint32_t)Rand_Next();
    sprite->var[0] = (s16)r;
    return r;
}

/* Enemy_Biplane_NextPropFrame - red biplane anim switch native: steps sprite var 0 and picks one
 * of 4 propeller frames: bit 0 alternates the two blur frames, +2 (the bright-hub frames) on the
 * 4 of every 32 steps whose bits 2-4 are 0
 * In:  sprite = the plane's sprite block (a5)
 * Out: d0.w = 0-3 (the case of Op2_CallNativeRelSwitch), d1.w = the step's bits 2-4; sprite var 0
 *      + 1
 * Evidence: addq.w #1,$22(a5) / move.w $22(a5),d0 / move.w d0,d1 / andi.w #1,d0 / andi.w #$1C,d1 /
 *           bne / addq.w #2,d0; op $2D30 at $02832E/$0283B6 in Scr2_028326; cases draw frames
 *           $38CC/$38D6 (blur) and $38E0/$38EA (bright hub)
 */
void Enemy_Biplane_NextPropFrame(uint16_t *d0, uint16_t *d1, Vm2Sprite *sprite)
{
    sprite->var[0] = (s16)(sprite->var[0] + 1);
    uint16_t step = (uint16_t)sprite->var[0];
    *d0 = (uint16_t)(step & 1);
    *d1 = (uint16_t)(step & 0x1C);
    if (*d1 == 0)
        *d0 = (uint16_t)(*d0 + 2);
}
