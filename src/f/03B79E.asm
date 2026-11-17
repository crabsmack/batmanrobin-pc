; ---------------------------------------------------------------------------
; Boss_CatHead_EyeSparkLeft - spawns a spark (a4 script $03B7F2: jittered heading, radius out, frame $15314) at the cat-head's left eye (x -$1E, y -$F, through the sway), heading = the head's + the shear x 256 + the script's word
; In:  a6 = the cat-head, a3 = its motion block (+$C heading), a2 = the Op2_CallNativeRel operand (word heading offset)
; Out: a2 past the word; Fx_RunScript's results with a0 = $03B7F2, d2 = 0, d3 = the heading, d4/d5 = the position
; Evidence: move.w #-$1E,d0 / lea $3B7F2,a0 / bra.s to the shared tail (Boss_CatHead_SwayPoint, jmp Fx_RunScript); dc.w offsets of Op2_CallNativeRel at $03C44C, $03C464 in the cat-head boss's attack coroutines ($03C43C-$03C47E: Op2_CallNativeRel with a word operand, two eyes per volley); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_CatHead_EyeSparkLeft:
	move.w	#$FFE2,d0
	lea.l	(Scr4_03B7F2).l,a0
	bra.s	Boss_CatHead_EyeFxAt
