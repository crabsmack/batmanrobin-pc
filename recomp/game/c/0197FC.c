#include "game.h"

/* the boss script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_01980A = 0x01980Au;

/* Boss_Harley_SetAimFromArg - native fragment of the Harley boss script: sets the aim target (sprite
 * variable 1) to twice the script argument g_VM2Arg0 (24 directions in 48 half-steps), then resumes
 * the script
 * In:  g_VM2Arg0 high word = direction 0-23; spr = the boss's sprite block (a5); the rest the
 *      object's registers (for the script)
 * Out: spr->var[1] = d0 low word = 2 * arg; the script continues at Scr2_01980A (jsr $7FE.w)
 * Evidence: move.w g_VM2Arg0,d0 / add.w d0,d0 / move.w d0,$24(a5) / jsr $7FE; Boss_Harley_TurnToAim
 *           turns $22(a5) toward it; TAS f4980-f5064 (8 frames), 3 runs. d0 is an output (the
 *           measured contract ends at the jsr $7FE.w; dk::jump_script hands on only outputs)
 */
void Boss_Harley_SetAimFromArg(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                               uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, Vm2Sprite *spr,
                               uint32_t a6)
{
    uint16_t aim = (uint16_t)((g_VM2Arg0 >> 16) * 2);   /* the word at g_VM2Arg0: the long's high half */
    *d0 = (*d0 & 0xFFFF0000u) | aim;
    spr->var[1] = aim;
    dk::jump_script(at_VM2_Enter, Scr2_01980A);
}
