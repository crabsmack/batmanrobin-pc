; ---------------------------------------------------------------------------
; Op2_ClearVelY - a2 op: sets the y velocity $1C(a3) to 0
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: clr.l $1C(a3); 2 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_ClearVelY:
	clr.l	$1C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
