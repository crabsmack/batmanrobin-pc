; ---------------------------------------------------------------------------
; Continue_SlowWhiteFade - after the continue countdown ran out and Scene_EndAfterDelayWhite started the white fade: sets the fade delay to 9 (a slow fade), then the script loops random shakes and white flashes until the scene ends
; In:  a2 script context (coroutine 0 of Scr2_0491A0, Op2_Native at $493D0)
; Out: g_FadeDelay = 9; jsr $7FE.w (Scr2_0493DC: Shake_Start, random Pal_FlashWhite, wait, loop)
; Evidence: move.b #9,g_FadeDelay; follows `2CBC 00009D24 0000 0000` at $493C6; ran only in scenario-game-over at f42783, after the countdown's shakes/flashes from $49348
; ---------------------------------------------------------------------------
Continue_SlowWhiteFade:
	move.b	#$9,(g_FadeDelay).w
	jsr	($7FE).w
