; ---------------------------------------------------------------------------
; PlayerSelect_ToggleHero - player-select native: switches the chosen hero (Batman <-> Robin) after an A/B/C/Up/Down press
; In:  a2 script context (Op2_Native at $49B70, after Op2_SetTimer0 $12C0)
; Out: g_P1IsRobin inverted; jsr $7FE.w (Op2_Return)
; Evidence: not.b g_P1IsRobin; ran only in scenario-robin (f536), where player 1 then plays Robin
; ---------------------------------------------------------------------------
PlayerSelect_ToggleHero:
	not.b	(g_P1IsRobin).w
	jsr	($7FE).w
