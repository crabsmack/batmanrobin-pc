; ---------------------------------------------------------------------------
; Op2_LoopTimer1 - a2 op: counts timer 1 ($1C(a4)) down by $30(a4) and branches (back) while it is still running; at expiry clears it and continues; a timer already at 0 branches (zero test first)
; In:  a2 = script: word offset
; Out: branch via $2D22 while running, else NEXT after the offset
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_LoopTimer1:
	tst.w	$1C(a4)
	beq.w	Op2_Goto
; ---------------------------------------------------------------------------
; Op2_LoopTimer1NoZeroTest - a2 op: the Op2_LoopTimer1 op without its zero test: counts timer 1 ($1C(a4)) down by the tick $30(a4) and branches (back) while it is still running; a timer already at 0 falls through (cleared, NEXT after the offset)
; In:  a2 = script: word offset
; Out: branch via Op2_Goto while running, else NEXT after the offset
; Evidence: a computed-jump target in the corpus (script handler words point past Op2_LoopTimer1's tst/beq, `dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_LoopTimer1NoZeroTest:
	move.w	$30(a4),d0
	sub.w	d0,$1C(a4)
	bhi.w	Op2_Goto
	clr.w	$1C(a4)
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
