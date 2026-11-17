; ---------------------------------------------------------------------------
; Fade_StartOutBlack - starts the fade to black: target = -$E minus the largest line tint offset, so every channel of every line reaches black
; In:  tint records $F4C7-$F4D5
; Out: g_FadeTarget set, g_FadeDelay = 2, g_FadeDelayCount = 0; registers preserved
; Evidence: target = $F2 - Fade_MaxTint; callers Scene_FadeOutExit (jsr) and the pointer at $04A760 (script native); SceneCb_WaitFadeExit then waits for g_FadeLevelVInt to reach it; TAS 24 frames (scene ends)
; ---------------------------------------------------------------------------
Fade_StartOutBlack:
	movem.l	d0-d1/a0,-(a7)
	clr.b	(g_FadeDelayCount).w
	move.b	#$2,(g_FadeDelay).w
	bsr.w	Fade_MaxTint
	neg.b	d0
	addi.b	#$F2,d0
	move.b	d0,(g_FadeTarget).w
	movem.l	(a7)+,d0-d1/a0
	rts
