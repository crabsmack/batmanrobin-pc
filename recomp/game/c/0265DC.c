#include "game.h"

static constexpr int32_t LADDER_START_ANGLE = 0x60000;   /* 6.0 in 16.16: the swing's first angle */

/* Enemy_LadderThug_InitLadder - shared tail of the rope-ladder spawners: clears the new object's
 * swing block (+$10: swing angle, angular speed) and starts the swing at angle 6.0
 * In:  self = the new ladder object (a6, left by Obj_SpawnEnemyNOrQuit / Obj_SpawnBoss...)
 * Out: d0 = 0 (Z set: the spawn succeeded); a0 clobbered (the block)
 * Evidence: movea.w $10(a6),a0 / jsr ObjPool_Clear / move.l #$60000,(a0) / moveq #0,d0 / rts;
 *           jmp from Enemy_LadderThug_SpawnA/B/C and Boss_Stage2_SpawnEndLadder;
 *           Enemy_LadderThug_SwingLadder reads that block; TAS f24981-f34742 (8 spawns). The tag
 *           gains tst(d0.l): the measured ccout Z is the final moveq's
 */
uint32_t Enemy_LadderThug_InitLadder(LadderThugObj *self)
{
    uint32_t d0 = 0;
    uint32_t block = (uint32_t)(int32_t)self->ladder;   /* movea.w: sign-extended handle */
    ObjPool_Clear(&d0, &block);                         /* zeroes the $5A bytes, a0 kept */
    dk::handle<LadderSwing>(self->ladder)->angle = LADDER_START_ANGLE;
    return 0;
}
