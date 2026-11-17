; ---------------------------------------------------------------------------
; Op2_TickTimer3Branch - a2 op: counts timer 3 ($20(a4)) down by $30(a4) and branches when it expires this run; a timer already at 0 also branches (no zero test)
; In:  a2 = script: word offset
; Out: branch at expiry (timer cleared), else NEXT after the offset (Op2_SkipWord)
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_TickTimer3Branch:
	move.w	$30(a4),d0
	sub.w	d0,$20(a4)
	bhi.w	Op2_SkipWord
	clr.w	$20(a4)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
