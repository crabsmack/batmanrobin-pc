#include "game.h"

/* The scene-end countdown ($009CE6-$009D0D): the idle per-frame callback, the a2 native that arms
 * the countdown, and the callback that counts it down (Frame_End calls g_FrameCallback once a
 * frame); when it runs out it falls into Scene_FadeOutExit. */

/* SceneCb_None - the idle per-frame scene callback: returns at once.
 * In:  none
 * Out: none
 * Evidence: rts; Scene_InitCommon stores $9CE6 in g_FrameCallback; the countdown callbacks branch
 *           here while their timer is >= 0
 */
void SceneCb_None(void)
{
}

/* Scene_EndAfterDelay - a2 native: ends the scene after a delay (a fade to black) with a result
 * code, unless the scene already ended with result 1 (Start pressed): then the operands are skipped.
 * In:  script (a2) = word result, word frames
 * Out: g_SceneResult = result, then as Scene_StartEndTimer; frames (d0) = the delay; script past
 *      the 4 bytes; ccr N/V/C clear on both paths (cmpi.w #1 equal; move.l last), Z set when skipped
 * Evidence: cmpi.w #1,g_SceneResult / bne / addq.l #4,a2 / rts; move.w (a2)+,g_SceneResult /
 *           move.w (a2)+,d0 into Scene_StartEndTimer; op $2CBC `dc.l $9CE8` in 22 scripts
 *           $4853C-$4A7F2
 */
void Scene_EndAfterDelay(uint16_t *frames, const u16 **script, dk::Flags *ccr)
{
    *ccr = dk::Flags::compare(g_SceneResult, 1, 2, ccr->x);
    if (ccr->z) {
        *script += 2;
        return;
    }
    g_SceneResult = (uint16_t)*(*script)++;
    *frames = (uint16_t)*(*script)++;
    Scene_StartEndTimer(*frames);
    *ccr = dk::Flags::test(at_SceneCb_EndTimer, 4, ccr->x);   /* move.l #$9D08: the last move */
}

/* Scene_StartEndTimer - arms the end-of-scene countdown: g_SceneTimer = frames, the per-frame
 * callback SceneCb_EndTimer.
 * In:  frames (d0)
 * Out: g_SceneTimer, g_FrameCallback
 * Evidence: move.w d0,g_SceneTimer / move.l #$9D08,g_FrameCallback; the tail of Scene_EndAfterDelay
 *           and SceneCb_CheckGameOver (d0 = $78)
 */
void Scene_StartEndTimer(uint16_t frames)
{
    g_SceneTimer = frames;
    g_FrameCallback = at_SceneCb_EndTimer;
}

/* SceneCb_EndTimer - per-frame callback: counts g_SceneTimer down; when it goes negative, on into
 * Scene_FadeOutExit.
 * In:  g_SceneTimer
 * Out: g_SceneTimer - 1; then Scene_FadeOutExit when it went below 0
 * Evidence: subq.w #1,g_SceneTimer / bpl SceneCb_None (the rts), then the fall into $9D0E;
 *           installed by Scene_StartEndTimer
 */
void SceneCb_EndTimer(void)
{
    g_SceneTimer = (uint16_t)(g_SceneTimer - 1);
    if ((int16_t)g_SceneTimer >= 0)
        return;
    dk::jump(at_Scene_FadeOutExit);
}
