#include "game.h"

/* HUD_StartGoSignKeepLock - native callback: starts the GO! sign like HUD_StartGoSign, but with
 * g_GoSignKeepLock set, so its arrival leaves the camera lock alone.
 * In:  none
 * Out: g_GoSignKeepLock = $FF; then HUD_StartGoSign_set (d0 = 0, the GO! sign state)
 * Evidence: st.b (g_GoSignKeepLock).w / bra.s HUD_StartGoSign_set; only pointer: Scr2_0204D4
 *           (op $2CBC); TAS frame 9596
 */
uint32_t HUD_StartGoSignKeepLock(void)
{
    g_GoSignKeepLock = 0xFF;
    dk::jump(at_HUD_StartGoSign_set);
    return 0;   /* moveq #0,d0 in HUD_StartGoSign_set */
}
