#include "game.h"

/* HUD_GoSignLeave - sends the stopped GO! sign off to the right: speed +$1A00 and a hold of $47,
 * counted down by 2 a frame before it moves.
 * In:  none
 * Out: g_GoSignSpeed = $1A00, g_GoSignState = $47
 * Evidence: move.l #$1A00,(g_GoSignSpeed).w / move.w #$47,(g_GoSignState).w; only caller
 *           HUD_UpdateGoSign once the camera scrolled past g_GoSignCameraX
 */
void HUD_GoSignLeave(void)
{
    g_GoSignSpeed = 0x1A00;
    g_GoSignState = 0x47;
}
