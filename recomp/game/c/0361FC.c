#include "game.h"

/* the bomb's script after the native (the bytes after its jsr $7FE.w; the label calls it Scr4_) */
static constexpr uint32_t Scr4_036210 = 0x036210u;

/* Enemy_BouncingBomb_TakeKnockback - native when a player strikes a bouncing bomb: adds the struck
 * box's knockback push to the bomb's x velocity and target speed, batting it away
 * In:  motion = the bomb's motion block (a3); g_StruckBox (its knockback +$22); the rest the
 *      object's registers (for the script)
 * Out: motion x velocity and target speed += the knockback; the script continues at Scr4_036210
 * Evidence: movea.l g_StruckBox,a0 / move.l $22(a0),d0 / add.l d0,$18(a3) / add.l d0,$24(a3) /
 *           jsr ($7FE).w; a2 native at $361FA in Scr2_0361A8 after Op2_SwitchAttacker; TAS f86759
 *           (once). a0/d0 are no outputs of the measured contract (the script never reads them)
 */
void Enemy_BouncingBomb_TakeKnockback(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                      uint32_t d7, uint32_t a1, Vm2Motion *motion, uint32_t a4, uint32_t a5,
                                      uint32_t a6)
{
    PTR(const HitBox) box;
    box.v = g_StruckBox;
    uint32_t push = (uint32_t)box->knockback;
    motion->vel_x = (int32_t)((uint32_t)motion->vel_x + push);
    motion->target_x = (int32_t)((uint32_t)motion->target_x + push);
    dk::jump_script(at_VM2_Enter, Scr4_036210);
}
