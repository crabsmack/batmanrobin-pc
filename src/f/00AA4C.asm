; ---------------------------------------------------------------------------
; Fade_StartOutWhite - starts the fade to white: target = +$E minus the smallest line tint offset, so every channel of every line reaches white
; In:  tint records $F4C7-$F4D5
; Out: g_FadeTarget set, g_FadeDelay = 2, g_FadeDelayCount = 0; registers preserved
; Evidence: target = $E - Fade_MinTint; only caller SceneCb_EndTimerWhite (installed by Scene_EndAfterDelayWhite); 4 corpus runs, not the TAS
; ---------------------------------------------------------------------------
Fade_StartOutWhite:
	movem.l	d0-d1/a0,-(a7)
	clr.b	(g_FadeDelayCount).w
	move.b	#$2,(g_FadeDelay).w
	bsr.w	Fade_MinTint
	neg.b	d0
	addi.b	#$E,d0
	move.b	d0,(g_FadeTarget).w
	movem.l	(a7)+,d0-d1/a0
	rts
