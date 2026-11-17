#include "game.h"

/* the firing animation after the native's jsr $7FE.w: the palette flash and the shot effect */
static constexpr uint32_t Scr2_0468A8 = 0x0468A8u;

enum { FREEZE_RECOIL_DOWN = 3 };   /* y impulse, px a frame (the integer word of the 16.16 impulse) */

/* Boss_Freeze_ShotRecoilDown - native step of one of Mr. Freeze's firing animations: the recoil pushes
 * him down (y impulse +3), then the animation goes on
 * In:  motion = the boss's motion block (a3: +$6 y impulse); the animation's registers
 * Out: the integer word of the y impulse = 3 (its fraction stays); the animation continues at
 *      Scr2_0468A8 (jsr $7FE.w)
 * Evidence: move.w #3,$6(a3) / jsr $7FE.w; entered by Op2_Native ($07FC) at $04689C; the bytes after it:
 *           Op2_CallNative Fade_InFromPale6 ($AB24) and the shot effect, as Boss_Freeze_ShotRecoilRight's;
 *           no corpus run reaches it (raw-stubs 2026-10-30); the contract is Boss_Freeze_ShotRecoilRight's;
 *           04689E.syn checks it
 */
void Boss_Freeze_ShotRecoilDown(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                uint32_t d6, uint32_t d7, uint32_t a1, Vm2Motion *motion, uint32_t a4, uint32_t a5,
                                uint32_t a6)
{
    uint32_t impulse = (uint32_t)(int32_t)motion->impulse_y;
    motion->impulse_y = (int32_t)(((uint32_t)FREEZE_RECOIL_DOWN << 16) | (impulse & 0xFFFFu));
    dk::jump_script(at_VM2_Enter, Scr2_0468A8);
}
