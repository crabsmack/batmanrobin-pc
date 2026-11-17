; ---------------------------------------------------------------------------
; Scene_FadeOutExit - starts the fade to black and installs SceneCb_WaitFadeExit, which leaves the scene loop once the fade is complete
; In:  none
; Out: $F4B8 = the new g_FadeTarget, g_FrameCallback = $9D54
; Evidence: $AA2A sets g_FadeTarget toward -$E (black: Scene_InitCommon fills CRAM black below level $E); callers Pause_Update (skip code), SceneCb_EndTimer, SceneCb_WaitPlayersExit, the debug menu $1569C and $47C6A (Start in the intro); 24 TAS frames
; ---------------------------------------------------------------------------
Scene_FadeOutExit:
	jsr	(Fade_StartOutBlack).l
	move.b	(g_FadeTarget).w,(g_ExitFadeTarget).w
	move.l	#$9D54,(g_FrameCallback).w
	rts
