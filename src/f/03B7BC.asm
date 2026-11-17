; ---------------------------------------------------------------------------
; Boss_CatHead_EyeShotRight - spawns a shot (a4 script $03B812) from the cat-head's right eye (x +$1E, y -$F, through the sway), as Boss_CatHead_EyeShotLeft
; In:  as Boss_CatHead_EyeSparkLeft
; Out: as Boss_CatHead_EyeShotLeft
; Evidence: move.w #$1E,d0, falls into the lea $3B812,a0 and the shared tail; Op2_CallNativeRel at $03C476 in the cat-head boss's attack coroutines ($03C43C-$03C47E: Op2_CallNativeRel with a word operand, two eyes per volley); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_CatHead_EyeShotRight:
	move.w	#$1E,d0
