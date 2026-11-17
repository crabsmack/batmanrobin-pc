; ---------------------------------------------------------------------------
; Fade_InFromPale6 - jumps the palette to a pale level (+$A toward white) and fades it back to normal one step per 6 frames, without the CRAM white flash
; In:  none
; Out: g_FadeDelay = g_FadeDelayCount = 5, g_FadeLevel = $A, g_FadeTarget = 0
; Evidence: pointers at $0467CC, $046836, $0468AA, $046900 (script natives in $0465E4); 3 corpus runs, TAS once at frame 130232 (the Mr. Freeze fight, png f130200)
; ---------------------------------------------------------------------------
Fade_InFromPale6:
	move.b	#$5,(g_FadeDelayCount).w
	move.b	#$5,(g_FadeDelay).w
	move.b	#$A,(g_FadeLevel).w
	clr.b	(g_FadeTarget).w
	rts
