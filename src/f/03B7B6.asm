; ---------------------------------------------------------------------------
; Boss_CatHead_EyeShotLeft - spawns a shot (a4 script $03B812: sound $58, damage $10, radius velocity, terrain move) from the cat-head's left eye (x -$1E, y -$F, through the sway)
; In:  as Boss_CatHead_EyeSparkLeft
; Out: as Boss_CatHead_EyeSparkLeft, with a0 = $03B812
; Evidence: move.w #-$1E,d0 / bra.s to the lea $3B812,a0 and the shared tail; Op2_CallNativeRel at $03C47C in the cat-head boss's attack coroutines ($03C43C-$03C47E: Op2_CallNativeRel with a word operand, two eyes per volley); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_CatHead_EyeShotLeft:
	move.w	#$FFE2,d0
	bra.s	Boss_CatHead_EyeShotAt
