#include "game.h"

/* Robin's script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_04FE88 = 0x04FE88u;

enum { PADVAR_HELD = 0, PADVAR_POSE = 2 };   /* the script variables the jetpack poses use */

/* Player_CopyPadToVar2Robin - native of Robin's script in the jetpack tunnel sections: copies script
 * variable 0 (the held pad bits) to variable 2, which the next op switches on to pick the pose (Batman's
 * Player_CopyPadToVar2 in Robin's script)
 * In:  block (a4) = Robin's script block; the other registers pass through untouched
 * Out: block->var[2] = block->var[0]; the script continues at Scr2_04FE88
 * Evidence: move.w $22(a4),$26(a4) / jsr $7FE.w; Op2_Native at $04FE7C in Robin's script (Scr2_04D80C on)
 *           after BranchVar1None $20 / AndVar1 $FFDF, as Batman's at $04D340; no corpus run reaches it
 *           (raw-stubs 2026-10-30); the contract and the C are Player_CopyPadToVar2's; 04FE7E.syn checks it
 */
void Player_CopyPadToVar2Robin(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                               uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *block, uint32_t a5,
                               uint32_t a6)
{
    block->var[PADVAR_POSE] = block->var[PADVAR_HELD];
    dk::jump_script(at_VM2_Enter, Scr2_04FE88);
}
