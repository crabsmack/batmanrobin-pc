#include "game.h"

/* the firing animation after the native's jsr $7FE.w: the palette flash (Fade_InFromPale6) and the
   shot effect Scr4 $014A4C */
static constexpr uint32_t Scr2_046834 = 0x046834u;

enum { FREEZE_RECOIL_RIGHT = 3 };   /* x impulse, px a frame (the integer word of the 16.16 impulse) */

/* Boss_Freeze_ShotRecoilRight - native step of Mr. Freeze's flipped (facing left) firing animation:
 * the recoil pushes him right (x impulse +3), then the animation goes on
 * In:  motion = the boss's motion block (a3: +$2 x impulse); the animation's registers
 * Out: the integer word of the x impulse = 3 (its fraction stays); the animation continues at
 *      Scr2_046834 (jsr $7FE.w)
 * Evidence: move.w #3,$2(a3) / jsr $7FE.w; entered by Op2_Native ($07FC) at $046828; the bytes after
 *           it: 2CBC Fade_InFromPale6, 51F2 E0F0 $00014A4C; the unflipped twin at $0467BE (impulse
 *           -3) is still raw and never ran; TAS f130232 only
 */
void Boss_Freeze_ShotRecoilRight(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                 uint32_t d6, uint32_t d7, uint32_t a1, Vm2Motion *motion, uint32_t a4, uint32_t a5,
                                 uint32_t a6)
{
    uint32_t impulse = (uint32_t)(int32_t)motion->impulse_x;
    motion->impulse_x = (int32_t)(((uint32_t)FREEZE_RECOIL_RIGHT << 16) | (impulse & 0xFFFFu));
    dk::jump_script(at_VM2_Enter, Scr2_046834);
}
