; ---------------------------------------------------------------------------
; Op2_FxHeadingHere - a2 op: sets the effect-spawn heading d3 to the object's current heading $C(a3) (high word)
; In:  a2 = script (no operand)
; Out: d3 = heading << 16; NEXT
; Evidence: move.w $C(a3),d3 / swap d3; 19 corpus runs, 90 script uses; $303BA `52F0 5198 000E`
; ---------------------------------------------------------------------------
Op2_FxHeadingHere:
	move.w	$C(a3),d3
	swap	d3
	movea.w	(a2)+,a0
	jmp	(a0)
