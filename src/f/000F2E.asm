; ---------------------------------------------------------------------------
; Op2_LoopTimer0 - a2 op: counts timer 0 ($1A(a4)) down by $30(a4) and branches (back) while it is still running; at expiry clears it and continues; a timer already at 0 falls through
; In:  a2 = script: word offset
; Out: branch via $2D22 while running, else NEXT after the offset
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_LoopTimer0:
	move.w	$30(a4),d0
	sub.w	d0,$1A(a4)
	bhi.w	Op2_Goto
	clr.w	$1A(a4)
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
