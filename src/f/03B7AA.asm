; ---------------------------------------------------------------------------
; Boss_CatHead_EyeSparkRight - spawns a spark (a4 script $03B7F2) at the cat-head's right eye (x +$1E, y -$F, through the sway), as Boss_CatHead_EyeSparkLeft
; In:  as Boss_CatHead_EyeSparkLeft
; Out: as Boss_CatHead_EyeSparkLeft
; Evidence: move.w #$1E,d0 / lea $3B7F2,a0 / bra.s the shared tail; Op2_CallNativeRel at $03C446, $03C45E in the cat-head boss's attack coroutines ($03C43C-$03C47E: Op2_CallNativeRel with a word operand, two eyes per volley); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_CatHead_EyeSparkRight:
	move.w	#$1E,d0
	lea.l	(Scr4_03B7F2).l,a0
	bra.s	Boss_CatHead_EyeFxAt
