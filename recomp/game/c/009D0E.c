#include "game.h"

/* Scene_FadeOutExit - starts the fade to black and installs SceneCb_WaitFadeExit, which leaves the
 * scene loop once the fade is complete.
 * In:  none
 * Out: g_ExitFadeTarget = the new g_FadeTarget; g_FrameCallback = SceneCb_WaitFadeExit; ccr N/Z/V/C
 *      clear (the move.l of the callback is the last move)
 * Evidence: jsr Fade_StartOutBlack / move.b g_FadeTarget,g_ExitFadeTarget / move.l
 *           #$9D54,g_FrameCallback; callers Pause_Update (skip code), SceneCb_EndTimer,
 *           SceneCb_WaitPlayersExit, the debug menu $1569C, $47C6A
 */
void Scene_FadeOutExit(dk::Flags *ccr)
{
    Fade_StartOutBlack(ccr);
    g_ExitFadeTarget = (uint8_t)g_FadeTarget;
    g_FrameCallback = at_SceneCb_WaitFadeExit;
    *ccr = dk::Flags::test(at_SceneCb_WaitFadeExit, 4, ccr->x);
}
