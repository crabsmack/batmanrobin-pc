; ---------------------------------------------------------------------------
; Op2_CruiseDown - a2 op: sets the y target speed $2C(a3) to +|y cruise speed $34(a3)| (accelerate downward); also the 'target is below' exit of the Y seek ops
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: move.l $34,$2C then neg when negative; branched to from $1A62-$1B70; entered by dispatch in 83 runs
; ---------------------------------------------------------------------------
Op2_CruiseDown:
	move.l	$34(a3),$2C(a3)
	bpl.w	VM2_Next
	neg.l	$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_CruiseUp - a2 op: sets the y target speed $2C(a3) to -|y cruise speed $34(a3)| (accelerate upward); also the 'target is above' exit of the Y seek ops
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: move.l $34,$2C then neg when positive; branched to from the Y seek ops; entered by dispatch in 75 runs
; ---------------------------------------------------------------------------
Op2_CruiseUp:
	move.l	$34(a3),$2C(a3)
	bmi.w	VM2_Next
	neg.l	$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
