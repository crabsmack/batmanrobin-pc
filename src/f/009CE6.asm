; ---------------------------------------------------------------------------
; SceneCb_None - the idle per-frame scene callback: returns at once (also the 'not yet' exit of the countdown callbacks)
; In:  none
; Out: none
; Evidence: Scene_InitCommon stores $9CE6 in g_FrameCallback $F604, which Frame_End calls every frame; SceneCb_EndTimer/SceneCb_EndTimerWhite branch here while their timer is >= 0; TAS f18-f456 (before the first stage installs another callback)
; ---------------------------------------------------------------------------
SceneCb_None:
	rts
; ---------------------------------------------------------------------------
; Scene_EndAfterDelay - a2 native: ends the scene after a delay (fade to black) with a result code, unless the scene already ended with result 1
; In:  a2 -> word result, word frames
; Out: g_SceneResult $F608 = result, timer $E034 = frames, g_FrameCallback = SceneCb_EndTimer; a2 past the 4 bytes
; Evidence: 22 scripts at $4853C-$4A7F2 call it through op $2CBC (`dc.l $9CE8`, e.g. $4853C: result 0, $12C = 300 frames); skips its args when g_SceneResult = 1 (Start already pressed)
; ---------------------------------------------------------------------------
Scene_EndAfterDelay:
	cmpi.w	#$1,(g_SceneResult).w
	bne.s	loc_009CF4
	addq.l	#$4,a2
	rts
loc_009CF4:
	move.w	(a2)+,(g_SceneResult).w
	move.w	(a2)+,d0
; ---------------------------------------------------------------------------
; Scene_StartEndTimer - arms the end-of-scene countdown: timer $E034 = d0 frames and g_FrameCallback = SceneCb_EndTimer
; In:  d0 = frames
; Out: $E034, $F604
; Evidence: shared tail of Scene_EndAfterDelay and SceneCb_CheckGameOver (d0 = $78)
; ---------------------------------------------------------------------------
Scene_StartEndTimer:
	move.w	d0,(g_SceneTimer).w
	move.l	#$9D08,(g_FrameCallback).w
	rts
; ---------------------------------------------------------------------------
; SceneCb_EndTimer - per-frame callback: counts the scene timer down and, when it runs out, falls into Scene_FadeOutExit
; In:  $E034 = frames left
; Out: $E034 - 1
; Evidence: subq.w #1,$E034 / bpl to the rts at $9CE6, then runs into $9D0E; installed by Scene_StartEndTimer; runtime seed $9D08
; ---------------------------------------------------------------------------
SceneCb_EndTimer:
	subq.w	#$1,(g_SceneTimer).w
	bpl.s	SceneCb_None
