; ---------------------------------------------------------------------------
; Fade_FlashWhite6 - white flash: CRAM white for 2 frames, then the palette fades in from full white (+$E) one step per 6 frames
; In:  none
; Out: g_FadeDelay = g_FadeDelayCount = 5, g_FadeLevel = $E, g_FadeTarget = 0, g_FlashReq set, g_FlashTimer = 2
; Evidence: Pal_VIntUpload fills CRAM with $0EEE while g_FlashTimer runs, then DMAs the faded palette; pointers in 8 stage/boss scripts ($025DD2, $026C46, $028EA0, $0305BA, $03CBFE, $03D190, $0433DA, $04649C: a2 native calls); 19 corpus runs, TAS 7 frames from 34683
; ---------------------------------------------------------------------------
Fade_FlashWhite6:
	move.b	#$5,(g_FadeDelayCount).w
	move.b	#$5,(g_FadeDelay).w
	move.b	#$E,(g_FadeLevel).w
	clr.b	(g_FadeTarget).w
	st.b	(g_FlashReq).w
	move.b	#$2,(g_FlashTimer).w
	rts
; ---------------------------------------------------------------------------
; Fade_FlashWhite7 - white flash: CRAM white for 2 frames, then the palette fades in from full white (+$E) one step per 7 frames
; In:  none
; Out: g_FadeDelay = g_FadeDelayCount = 6, g_FadeLevel = $E, g_FadeTarget = 0, g_FlashReq set, g_FlashTimer = 2
; Evidence: same as Fade_FlashWhite6 with delay 6; jsr from the native $029608 (with Tint_FlashC3) and a pointer at $03CC60; not run in the corpus
; ---------------------------------------------------------------------------
Fade_FlashWhite7:
	move.b	#$6,(g_FadeDelayCount).w
	move.b	#$6,(g_FadeDelay).w
	move.b	#$E,(g_FadeLevel).w
	clr.b	(g_FadeTarget).w
	st.b	(g_FlashReq).w
	move.b	#$2,(g_FlashTimer).w
	rts
; ---------------------------------------------------------------------------
; Fade_FlashWhite3 - white flash: CRAM white for 2 frames, then the palette fades in from full white (+$E) one step per 3 frames
; In:  none
; Out: g_FadeDelay = g_FadeDelayCount = 2, g_FadeLevel = $E, g_FadeTarget = 0, g_FlashReq set, g_FlashTimer = 2
; Evidence: same as Fade_FlashWhite6 with delay 2; pointers at $01D8A6, $026B4C, $0295F8, $033C30, $03634E, $03C76E, $03CC14, $0422F8 (script natives); not run in the corpus
; ---------------------------------------------------------------------------
Fade_FlashWhite3:
	move.b	#$2,(g_FadeDelayCount).w
	move.b	#$2,(g_FadeDelay).w
	move.b	#$E,(g_FadeLevel).w
	clr.b	(g_FadeTarget).w
	st.b	(g_FlashReq).w
	move.b	#$2,(g_FlashTimer).w
	rts
