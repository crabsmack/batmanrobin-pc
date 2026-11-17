#include "game.h"

/* the enemy body's script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_035AF6 = 0x035AF6u;

/* Enemy_BoostImpulseX - a2 native right after Op2_TakeHit in the card-hall enemy body $035AA6:
 * adds a quarter of the motion block's x impulse to itself (knockback x 1.25), then continues
 * the script
 * In:  motion = the struck enemy's motion block (a3: +2 the x impulse, 16.16); the rest the
 *      object's registers (for the script)
 * Out: motion x impulse += impulse >> 2 (arithmetic); the script continues at Scr2_035AF6
 * Evidence: move.l 2(a3),d0 / asr.l #2,d0 / add.l d0,2(a3) / jsr ($7FE).w; native word at $35AE6
 *           in the body $035AA6; TAS f111035-f112744 (5 hits). d0 is no output of the measured
 *           contract (the script never reads it)
 */
void Enemy_BoostImpulseX(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                         uint32_t d7, uint32_t a1, Vm2Motion *motion, uint32_t a4, uint32_t a5, uint32_t a6)
{
    int32_t impulse = motion->impulse_x;
    motion->impulse_x = (int32_t)((uint32_t)impulse + (uint32_t)(impulse >> 2));
    dk::jump_script(at_VM2_Enter, Scr2_035AF6);
}
