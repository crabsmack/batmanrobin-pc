; ---------------------------------------------------------------------------
; Boss_Harley_SetAimFromArg - native fragment of the Harley boss script: sets the aim target $24(a5) to twice the script argument g_VM2Arg0 (24 directions in 48 half-steps), then resumes the script
; In:  g_VM2Arg0 = direction 0-23
; Out: $24(a5) = 2 * arg
; Evidence: move.w g_VM2Arg0,d0 / add.w d0,d0 / move.w d0,$24(a5) / jsr $7FE; Boss_Harley_TurnToAim turns $22(a5) toward it; TAS f4980-f5064 (8 frames), 3 runs
; ---------------------------------------------------------------------------
Boss_Harley_SetAimFromArg:
	move.w	(g_VM2Arg0).w,d0
	add.w	d0,d0
	move.w	d0,$24(a5)
	jsr	($7FE).w
