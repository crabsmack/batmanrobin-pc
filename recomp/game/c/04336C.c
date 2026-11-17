#include "game.h"

/* Enemy_BombBot_FrameByCameraX - which of the bomb bot's two rolling frames ($4A1A / $4A24) to draw:
 * bit 1 of the camera's x, so the wheel turns as the screen scrolls
 * In:  g_CameraX (its integer word)
 * Out: d0 = 0 or 1 (the animation's Op2_CallNativeRelSwitch case)
 * Evidence: move.w g_CameraX,d0 / lsr.w #1 / andi.w #1; only reference Op2_CallNativeRelSwitch at
 *           $4337E in the bomb bot animation Scr4_043378; TAS 116192-122471 (stage 4 hall and lab)
 */
uint16_t Enemy_BombBot_FrameByCameraX(void)
{
    uint16_t camera_x = (uint16_t)(g_CameraX >> 16);
    return (uint16_t)((camera_x >> 1) & 1);
}
