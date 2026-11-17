#include "game.h"

/* The scene-exit natives and per-frame callbacks ($009D24-$009DC5): the fade-to-white countdown,
 * the wait for the exit fade (which returns from the scene loop), the game-over check and the
 * section exit that waits for the players to walk off. */

/* Scene_EndAfterDelayWhite - a2 native: ends the scene after a delay with a fade to white and a
 * result code.
 * In:  script (a2) = word result, word frames
 * Out: g_SceneResult = result, g_SceneTimer = frames (also d0), g_FrameCallback =
 *      SceneCb_EndTimerWhite; script past the 4 bytes; ccr N/Z/V/C clear (the move.l last)
 * Evidence: move.w (a2)+,g_SceneResult / move.w (a2)+,d0 / move.w d0,g_SceneTimer / move.l
 *           #$9D38,g_FrameCallback; scripts $4872E, $493C8 through op $2CBC
 */
void Scene_EndAfterDelayWhite(uint16_t *frames, const u16 **script, dk::Flags *ccr)
{
    g_SceneResult = (uint16_t)*(*script)++;
    *frames = (uint16_t)*(*script)++;
    g_SceneTimer = *frames;
    g_FrameCallback = at_SceneCb_EndTimerWhite;
    *ccr = dk::Flags::test(at_SceneCb_EndTimerWhite, 4, ccr->x);
}

/* SceneCb_EndTimerWhite - per-frame callback: counts g_SceneTimer down; when it goes negative,
 * starts the fade to white and installs SceneCb_WaitFadeExit.
 * In:  g_SceneTimer
 * Out: g_SceneTimer - 1; then g_ExitFadeTarget = the fade's target, g_FrameCallback
 * Evidence: subq.w #1 / bpl SceneCb_None; jsr Fade_StartOutWhite / move.b g_FadeTarget,
 *           g_ExitFadeTarget / move.l #$9D54,g_FrameCallback; installed by Scene_EndAfterDelayWhite
 */
void SceneCb_EndTimerWhite(void)
{
    g_SceneTimer = (uint16_t)(g_SceneTimer - 1);
    if ((int16_t)g_SceneTimer >= 0)
        return;                                   /* bpl SceneCb_None: its rts */
    Fade_StartOutWhite();
    g_ExitFadeTarget = (uint8_t)g_FadeTarget;
    g_FrameCallback = at_SceneCb_WaitFadeExit;
}

/* SceneCb_WaitFadeExit - per-frame callback: holds the exit fade's target (2 frames a step) and,
 * once the V-int's fade level reaches it, turns the V-int off and returns past Frame_End and the
 * scene loop's call of it, so the scene's caller (Scene_RunList / Section_RunNext) continues.
 * In:  g_ExitFadeTarget, g_FadeLevelVInt
 * Out: g_FadeDelay = 2, g_FadeTarget = the target (also the result: d0's low byte); reached:
 *      g_VIntOff = 1 and two return addresses dropped; ccr: cmp.b of target and level, or move.b's
 * Evidence: move.b #2,g_FadeDelay / move.b g_ExitFadeTarget,d0 / move.b d0,g_FadeTarget / cmp.b
 *           g_FadeLevelVInt,d0 / bne / move.b #1,g_VIntOff / addq.l #8,a7 / rts; installed by
 *           Scene_FadeOutExit, SceneCb_EndTimerWhite
 */
uint8_t SceneCb_WaitFadeExit(dk::Flags *ccr)
{
    g_FadeDelay = 2;
    uint8_t target = g_ExitFadeTarget;
    g_FadeTarget = target;
    *ccr = dk::Flags::compare(target, g_FadeLevelVInt, 1, ccr->x);
    if (!ccr->z)
        return target;
    g_VIntOff = 1;
    *ccr = dk::Flags::test(1, 1, ccr->x);
    dk::drop_return();                            /* Frame_End's jsr (a0) */
    dk::drop_return();                            /* the scene loop's jsr Frame_End */
    return target;
}

/* SceneCb_CheckGameOver - gameplay per-frame callback: when neither player is in play, locks
 * joining, sets the result to 2 (game over) and arms a 120-frame end timer.
 * In:  g_P1Flags, g_P2Flags (bit 0 = in play)
 * Out: game over: g_JoinLocked set, g_SceneResult = 2, frames (d0) = $78, then Scene_StartEndTimer
 * Evidence: btst #0,g_P1Flags / bne / btst #0,g_P2Flags / bne / st.b g_JoinLocked / move.w
 *           #2,g_SceneResult / move.w #$78,d0 / bra.w Scene_StartEndTimer; installed by
 *           Scene_InitStage; Main_Start treats result 2 as game over
 */
void SceneCb_CheckGameOver(uint16_t *frames)
{
    if (g_P1Flags & 1)
        return;
    if (g_P2Flags & 1)
        return;
    g_JoinLocked = 0xFF;
    g_SceneResult = 2;
    *frames = 0x78;
    dk::jump(at_Scene_StartEndTimer);
}

/* bit 12 of a player's variable 1: at the exit (andi.w #$1000,d0 keeps only it in d0) */
static bool at_exit(uint16_t *var)
{
    *var &= 0x1000;
    return *var != 0;
}

/* SceneCb_WaitPlayersExit - section-exit per-frame callback: on frames where every player in play
 * has bit 12 of its script variable 1 set (it reached the exit), counts g_ExitTimer down; at 0,
 * Scene_FadeOutExit ends the section. Nothing in play, or the timer already 0: nothing.
 * In:  g_ExitTimer; the players' script variable 1 (Player_GetP1Var1 / P2Var1)
 * Out: g_ExitTimer - 1; var (d0) / one (d1) / vm (a4) as the last Player_GetPnVar1 left them
 * Evidence: tst.w g_ExitTimer / beq; bsr Player_GetP1Var1 / bne; andi.w #$1000,d0 / beq; subq.w
 *           #1,g_ExitTimer / ble.w Scene_FadeOutExit; installed by $1054A after it sets bit $800 of
 *           each player's $24(a4) (NOTES)
 */
void SceneCb_WaitPlayersExit(uint16_t *var, uint32_t *one, uint32_t *vm)
{
    dk::Flags in_play{};
    if (g_ExitTimer == 0)
        return;
    Player_GetP1Var1(var, one, vm, &in_play);
    if (!in_play.z) {
        if (!at_exit(var))
            return;                               /* player 1 not at the exit yet */
        Player_GetP2Var1(var, one, vm, &in_play);
        if (!in_play.z && !at_exit(var))
            return;                               /* player 2 in play, not at the exit */
    } else {
        Player_GetP2Var1(var, one, vm, &in_play);
        if (in_play.z)
            return;                               /* nobody in play */
        if (!at_exit(var))
            return;
    }
    int32_t left = (int16_t)g_ExitTimer - 1;
    g_ExitTimer = (uint16_t)left;
    if (left <= 0)
        dk::jump(at_Scene_FadeOutExit);
}
