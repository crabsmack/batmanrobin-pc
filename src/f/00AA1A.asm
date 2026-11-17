; ---------------------------------------------------------------------------
; Fade_ToNormal - fades the palette back to normal brightness from its current level, one step per 3 frames
; In:  none
; Out: g_FadeTarget = 0, g_FadeDelay = 2, g_FadeDelayCount = 0
; Evidence: pointer at $04A778 (a2 native call from a scene script); ran in 2 corpus runs, not in the TAS; same delay as Fade_StartIn but keeps the current g_FadeLevel
; ---------------------------------------------------------------------------
Fade_ToNormal:
	clr.b	(g_FadeDelayCount).w
	move.b	#$2,(g_FadeDelay).w
	clr.b	(g_FadeTarget).w
	rts
