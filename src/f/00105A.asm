; ---------------------------------------------------------------------------
; Op2_SetTimer2 - a2 op: sets timer 2 ($1E(a4)) from a word operand
; In:  a2 = script: word ticks (counted down by $30(a4) per run)
; Out: NEXT
; Evidence: counted down by Op2_TickTimer2Branch / Op2_LoopTimer2; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer2:
	move.w	(a2)+,$1E(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTimer2Rand - a2 op: sets timer 2 ($1E(a4)) to base + random(range) (Rand_Below)
; In:  a2 = script: words range, base
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer2Rand:
	move.w	(a2)+,d0
	jsr	(Rand_Below).l
	add.w	(a2)+,d0
	move.w	d0,$1E(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_TickTimer2Branch - a2 op: counts timer 2 ($1E(a4)) down by $30(a4) and branches when it expires this run; a timer already at 0 skips the offset
; In:  a2 = script: word offset
; Out: branch at expiry (timer cleared), else NEXT after the offset (Op2_SkipWord)
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_TickTimer2Branch:
	tst.w	$1E(a4)
	beq.w	Op2_SkipWord
; ---------------------------------------------------------------------------
; Op2_TickTimer2BranchNoZeroTest - a2 op: the Op2_TickTimer2Branch op without its zero test: counts timer 2 ($1E(a4)) down by the tick $30(a4) and branches when it expires this run; a timer already at 0 also branches (0 - tick borrows)
; In:  a2 = script: word offset
; Out: branch at expiry (timer cleared), else NEXT after the offset (Op2_SkipWord)
; Evidence: a computed-jump target in the corpus (script handler words point past Op2_TickTimer2Branch's tst/beq, `dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_TickTimer2BranchNoZeroTest:
	move.w	$30(a4),d0
	sub.w	d0,$1E(a4)
	bhi.w	Op2_SkipWord
	clr.w	$1E(a4)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
