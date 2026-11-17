#include "game.h"

static constexpr uint16_t GOSIGN_START_X = 0x168;          /* off the right edge */
static constexpr uint32_t GOSIGN_ENTER_SPEED = 0xFFFC0000u; /* -4.0 px a frame (16.16) */
static constexpr uint16_t GOSIGN_MOVING = 0xFFFF;           /* g_GoSignState: moving */

/* HUD_StartGoSign - starts the GO! sign entering from the right, with the camera lock released on
 * its arrival.
 * In:  none
 * Out: g_GoSignKeepLock = 0; the state as HUD_StartGoSign_set
 * Evidence: sf.b (g_GoSignKeepLock).w, falling into HUD_StartGoSign_set; called by
 *           HUD_ShowGoSignCb; 20 times in the TAS (1388-120817)
 */
void HUD_StartGoSign(void)
{
    g_GoSignKeepLock = 0;
    HUD_StartGoSign_set();
}

/* HUD_StartGoSign_set - the two GO! sign starts' shared tail: x $168, speed -4.0, state moving,
 * and the camera x it waits for the player to scroll past.
 * In:  g_GoSignKeepLock set or cleared
 * Out: g_GoSignX high word = $168, g_GoSignSpeed = -4.0, g_GoSignState = -1, g_GoSignCameraX =
 *      g_CameraX's integer word; d0 = 0 (moveq: the flags tst.l)
 * Evidence: move.w #$168,(g_GoSignX).w / move.l #$FFFC0000,(g_GoSignSpeed).w / move.w
 *           #$FFFF,(g_GoSignState).w / move.w (g_CameraX).w,(g_GoSignCameraX).w / moveq #0,d0
 */
uint32_t HUD_StartGoSign_set(void)
{
    g_GoSignX = (uint32_t)(GOSIGN_START_X << 16) | (g_GoSignX & 0xFFFFu);   /* move.w: the word at +0 */
    g_GoSignSpeed = GOSIGN_ENTER_SPEED;
    g_GoSignState = GOSIGN_MOVING;
    g_GoSignCameraX = (uint16_t)(g_CameraX >> 16);
    return 0;
}
