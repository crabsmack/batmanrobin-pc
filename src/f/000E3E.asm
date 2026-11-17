; ---------------------------------------------------------------------------
; Op2_SetTimer3RandYield3 - a2 op for coroutine 3: sets timer 3 ($20(a4)) to base + random(range) (Rand_Below), saves the next op in slot 3 ($44(a4)) and returns to the code that stepped the coroutine
; In:  a2 = coroutine script: words range, base; (sp) = the stepping script's a2
; Out: a2 = popped caller script, NEXT (as the return op $2D2A)
; Evidence: coroutine slots $38/$3C/$40/$44(a4) are started by $2334-$240C (store a script address) and stepped by $235A/$23A2/$23EA/$2432 (push a2, a2 = slot, NEXT); $2D28 is an empty coroutine (its first op is the return $2D2A); the next op is typically Op2_WaitTimer3; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer3RandYield3:
	move.w	(a2)+,d0
	jsr	(Rand_Below).l
	add.w	(a2)+,d0
	move.w	d0,$20(a4)
	move.l	a2,$44(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_WaitTimer0 - a2 op for a coroutine: counts timer 0 ($1A(a4)) down by the tick $30(a4); while it is above 0 returns to the stepping script without moving the slot, so the coroutine re-runs this op on its next step; at 0 clears it and continues
; In:  a2 = coroutine script (no operand); (sp) = caller a2
; Out: return through $2D2A (movea.l (a7)+,a2) while running; NEXT once expired
; Evidence: pairs with the Op2_SetTimer0Yield* ops, which leave the slot pointing here; executed in the TAS
; ---------------------------------------------------------------------------
Op2_WaitTimer0:
	move.w	$30(a4),d0
	sub.w	d0,$1A(a4)
	bhi.w	Op2_Return
	clr.w	$1A(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_WaitTimer1 - a2 op for a coroutine: counts timer 1 ($1C(a4)) down by the tick $30(a4); while it is above 0 returns to the stepping script without moving the slot, so the coroutine re-runs this op on its next step; at 0 clears it and continues
; In:  a2 = coroutine script (no operand); (sp) = caller a2
; Out: return through $2D2A (movea.l (a7)+,a2) while running; NEXT once expired
; Evidence: pairs with the Op2_SetTimer1Yield* ops, which leave the slot pointing here; executed in the TAS
; ---------------------------------------------------------------------------
Op2_WaitTimer1:
	move.w	$30(a4),d0
	sub.w	d0,$1C(a4)
	bhi.w	Op2_Return
	clr.w	$1C(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_WaitTimer2 - a2 op for a coroutine: counts timer 2 ($1E(a4)) down by the tick $30(a4); while it is above 0 returns to the stepping script without moving the slot, so the coroutine re-runs this op on its next step; at 0 clears it and continues
; In:  a2 = coroutine script (no operand); (sp) = caller a2
; Out: return through $2D2A (movea.l (a7)+,a2) while running; NEXT once expired
; Evidence: pairs with the Op2_SetTimer2Yield* ops, which leave the slot pointing here; executed in the TAS
; ---------------------------------------------------------------------------
Op2_WaitTimer2:
	move.w	$30(a4),d0
	sub.w	d0,$1E(a4)
	bhi.w	Op2_Return
	clr.w	$1E(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_WaitTimer3 - a2 op for a coroutine: counts timer 3 ($20(a4)) down by the tick $30(a4); while it is above 0 returns to the stepping script without moving the slot, so the coroutine re-runs this op on its next step; at 0 clears it and continues
; In:  a2 = coroutine script (no operand); (sp) = caller a2
; Out: return through $2D2A (movea.l (a7)+,a2) while running; NEXT once expired
; Evidence: pairs with the Op2_SetTimer3Yield* ops, which leave the slot pointing here; executed in the TAS
; ---------------------------------------------------------------------------
Op2_WaitTimer3:
	move.w	$30(a4),d0
	sub.w	d0,$20(a4)
	bhi.w	Op2_Return
	clr.w	$20(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTimer0 - a2 op: sets timer 0 ($1A(a4)) from a word operand
; In:  a2 = script: word ticks (counted down by $30(a4) per run)
; Out: NEXT
; Evidence: counted down by Op2_TickTimer0Branch / Op2_LoopTimer0; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer0:
	move.w	(a2)+,$1A(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTimer0Rand - a2 op: sets timer 0 ($1A(a4)) to base + random(range) (Rand_Below)
; In:  a2 = script: words range, base
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer0Rand:
	move.w	(a2)+,d0
	jsr	(Rand_Below).l
	add.w	(a2)+,d0
	move.w	d0,$1A(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_TickTimer0Branch - a2 op: counts timer 0 ($1A(a4)) down by $30(a4) and branches when it expires this run; a timer already at 0 skips the offset
; In:  a2 = script: word offset
; Out: branch at expiry (timer cleared), else NEXT after the offset (Op2_SkipWord)
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_TickTimer0Branch:
	tst.w	$1A(a4)
	beq.w	Op2_SkipWord
; ---------------------------------------------------------------------------
; Op2_TickTimer0BranchNoZeroTest - a2 op: the Op2_TickTimer0Branch op without its zero test: counts timer 0 ($1A(a4)) down by the tick $30(a4) and branches when it expires this run; a timer already at 0 also branches (0 - tick borrows)
; In:  a2 = script: word offset
; Out: branch at expiry (timer cleared), else NEXT after the offset (Op2_SkipWord)
; Evidence: a computed-jump target in the corpus (script handler words point past Op2_TickTimer0Branch's tst/beq, `dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_TickTimer0BranchNoZeroTest:
	move.w	$30(a4),d0
	sub.w	d0,$1A(a4)
	bhi.w	Op2_SkipWord
	clr.w	$1A(a4)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_TickTimer0GotoV0 - a2 op: counts timer 0 ($1A(a4)) down by $30(a4); at expiry clears it and jumps to the object's script vector $32(a6), else continues
; In:  a2 = script (no operand); $32(a6) = script address set by $3A8E/$3A9A/$3ADE
; Out: NEXT either way (at the vector on expiry)
; Evidence: $3B06/$3B0E jump to the same vectors; executed in the TAS
; ---------------------------------------------------------------------------
Op2_TickTimer0GotoV0:
	move.w	$30(a4),d0
	sub.w	d0,$1A(a4)
	bhi.w	VM2_Next
	clr.w	$1A(a4)
	movea.l	$32(a6),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_TickTimer0GotoV1 - a2 op: counts timer 0 ($1A(a4)) down by $30(a4); at expiry clears it and jumps to the object's script vector $36(a6), else continues
; In:  a2 = script (no operand); $36(a6) = script address set by $3A8E/$3A9A/$3ADE
; Out: NEXT either way (at the vector on expiry)
; Evidence: $3B06/$3B0E jump to the same vectors; executed in the TAS
; ---------------------------------------------------------------------------
Op2_TickTimer0GotoV1:
	move.w	$30(a4),d0
	sub.w	d0,$1A(a4)
	bhi.w	VM2_Next
	clr.w	$1A(a4)
	movea.l	$36(a6),a2
	movea.w	(a2)+,a0
	jmp	(a0)
