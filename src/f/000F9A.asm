; ---------------------------------------------------------------------------
; Op2_TickTimer1Branch - a2 op: counts timer 1 ($1C(a4)) down by $30(a4) and branches when it expires this run; a timer already at 0 skips the offset
; In:  a2 = script: word offset
; Out: branch at expiry (timer cleared), else NEXT after the offset (Op2_SkipWord)
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_TickTimer1Branch:
	tst.w	$1C(a4)
	beq.w	Op2_SkipWord
; ---------------------------------------------------------------------------
; Op2_TickTimer1BranchNoZeroTest - a2 op: the Op2_TickTimer1Branch op without its zero test: counts timer 1 ($1C(a4)) down by the tick $30(a4) and branches when it expires this run; a timer already at 0 also branches (0 - tick borrows)
; In:  a2 = script: word offset
; Out: branch at expiry (timer cleared), else NEXT after the offset (Op2_SkipWord)
; Evidence: a computed-jump target in the corpus (script handler words point past Op2_TickTimer1Branch's tst/beq, `dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_TickTimer1BranchNoZeroTest:
	move.w	$30(a4),d0
	sub.w	d0,$1C(a4)
	bhi.w	Op2_SkipWord
	clr.w	$1C(a4)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
