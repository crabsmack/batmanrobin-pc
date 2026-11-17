#include "game.h"

/* Batman's script after the native (the bytes after its jsr $7FE.w): Op2_SwitchVar2Bits */
static constexpr uint32_t Scr2_04D34C = 0x04D34Cu;

enum { PADVAR_HELD = 0, PADVAR_POSE = 2 };   /* the script variables the jetpack poses use */

/* Player_CopyPadToVar2 - native of Batman's script in the jetpack tunnel sections: copies script
 * variable 0 (the held pad bits) to variable 2, which the next op switches on to pick the pose
 * In:  block (a4) = Batman's script block; the other registers pass through untouched
 * Out: block->var[2] = block->var[0]; the script continues at Scr2_04D34C
 * Evidence: move.w $22(a4),$26(a4) / jsr $7FE.w; inside Scr2_04ACD0 (Batman) after BranchVar1None
 *           $20 / AndVar1 $FFDF, followed by Scr2_04D34C Op2_SwitchVar2Bits -> SetAnim; TAS
 *           f101718-130436 (255 frames)
 */
void Player_CopyPadToVar2(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                          uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *block, uint32_t a5, uint32_t a6)
{
    block->var[PADVAR_POSE] = block->var[PADVAR_HELD];
    dk::jump_script(at_VM2_Enter, Scr2_04D34C);
}
