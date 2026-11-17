; ---------------------------------------------------------------------------
; Fade_StartIn - starts a fade in: puts the palette fully faded (white when g_FadeLevel is already at +$E, else black) and sets target 0 at one step per 3 frames
; In:  g_FadeLevel
; Out: g_FadeTarget = 0, g_FadeDelayCount = 0, g_FadeDelay = 2, g_FadeLevel = g_FadeLevelVInt = +$E or -$E; d0 trashed
; Evidence: tail of Scene_InitStage and Scene_InitPlain (bra); Scene_InitCommon clears CRAM to white or black on the same g_FadeLevel >= $E test; TAS 25 frames, one per scene start
; ---------------------------------------------------------------------------
Fade_StartIn:
	clr.b	(g_FadeTarget).w
	clr.b	(g_FadeDelayCount).w
	move.b	#$2,(g_FadeDelay).w
	move.b	#$E,d0
	cmp.b	(g_FadeLevel).w,d0
	ble.s	loc_00AA10
	neg.b	d0
loc_00AA10:
	move.b	d0,(g_FadeLevelVInt).w
	move.b	d0,(g_FadeLevel).w
	rts
