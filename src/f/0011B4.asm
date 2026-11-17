; ---------------------------------------------------------------------------
; Op2_LoopTimer3 - a2 op: counts timer 3 ($20(a4)) down by $30(a4) and branches (back) while it is still running; at expiry clears it and continues; a timer already at 0 branches (zero test first)
; In:  a2 = script: word offset
; Out: branch via $2D22 while running, else NEXT after the offset
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_LoopTimer3:
	tst.w	$20(a4)
	beq.w	Op2_Goto
; ---------------------------------------------------------------------------
; Op2_LoopTimer3NoZeroTest - a2 op: the Op2_LoopTimer3 op without its zero test: counts timer 3 ($20(a4)) down by the tick $30(a4) and branches (back) while it is still running; a timer already at 0 falls through (cleared, NEXT after the offset)
; In:  a2 = script: word offset
; Out: branch via Op2_Goto while running, else NEXT after the offset
; Evidence: a computed-jump target in the corpus (script handler words point past Op2_LoopTimer3's tst/beq, `dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_LoopTimer3NoZeroTest:
	move.w	$30(a4),d0
	sub.w	d0,$20(a4)
	bhi.w	Op2_Goto
	clr.w	$20(a4)
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
