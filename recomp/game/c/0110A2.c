#include "game.h"

/* HUD_Init - scene-start HUD reset: unlocks joining, makes the HUD art upload the next HUD DMA
 * step and starts both player panels' opening scripts.
 * In:  none
 * Out: g_JoinLocked = 0, g_HUDStep = 4
 * Evidence: sf.b (g_JoinLocked).w / move.w #4,(g_HUDStep).w / lea g_Player1,a0 / bsr HUD_InitPanel /
 *           lea g_Player2,a0 and the fall into HUD_InitPanel (a jump: a0, a clobber, is an output here so the jump hands it on); one caller, the scene setup $99D4
 */
void HUD_Init(uint32_t *a0)
{
    g_JoinLocked = 0;
    g_HUDStep = 4;
    HUD_InitPanel((PlayerRec *)g_Player1);
    *a0 = dk::addr(g_Player2);
    dk::jump(at_HUD_InitPanel);
}
