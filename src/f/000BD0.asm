; ---------------------------------------------------------------------------
; Op2_SetAnim - a2 op: starts a sprite animation: stores the relative-word target as the animation script pointer $2E(a5), resets its timer $1E(a5) and clears the done flag (bit 0 of $18(a5)); continues
; In:  a2 = script: word offset (from the operand) to the animation script
; Out: NEXT
; Evidence: pairs with Op2_BranchAnimFlag ($BE6), which branches on bit 0 of $18(a5) (the flag this op clears; that the animator sets it at the end is inferred); the file also holds the thread-slot ops $BF6-$C12; 8.8k TAS frames
; ---------------------------------------------------------------------------
Op2_SetAnim:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	move.l	a0,$2E(a5)
	clr.w	$1E(a5)
	bclr	#$0,$18(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchAnimFlag - a2 op: branches when bit 0 of $18(a5), the flag Op2_SetAnim clears, is set (animation finished, inferred)
; In:  a2 = script: word offset
; Out: branch, else NEXT after the offset
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_BranchAnimFlag:
	btst	#$0,$18(a5)
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTimer0Yield0 - a2 op for coroutine 0: sets timer 0 ($1A(a4)) to the word operand, saves the next op in slot 0 ($38(a4)) and returns to the code that stepped the coroutine
; In:  a2 = coroutine script: word ticks; (sp) = the stepping script's a2
; Out: a2 = popped caller script, NEXT (as the return op $2D2A)
; Evidence: coroutine slots $38/$3C/$40/$44(a4) are started by $2334-$240C (store a script address) and stepped by $235A/$23A2/$23EA/$2432 (push a2, a2 = slot, NEXT); $2D28 is an empty coroutine (its first op is the return $2D2A); the next op is typically Op2_WaitTimer0; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer0Yield0:
	move.w	(a2)+,$1A(a4)
	move.l	a2,$38(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTimer1Yield0 - a2 op for coroutine 0: sets timer 1 ($1C(a4)) to the word operand, saves the next op in slot 0 ($38(a4)) and returns to the code that stepped the coroutine
; In:  a2 = coroutine script: word ticks; (sp) = the stepping script's a2
; Out: a2 = popped caller script, NEXT (as the return op $2D2A)
; Evidence: coroutine slots $38/$3C/$40/$44(a4) are started by $2334-$240C (store a script address) and stepped by $235A/$23A2/$23EA/$2432 (push a2, a2 = slot, NEXT); $2D28 is an empty coroutine (its first op is the return $2D2A); the next op is typically Op2_WaitTimer1; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer1Yield0:
	move.w	(a2)+,$1C(a4)
	move.l	a2,$38(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTimer2Yield0 - a2 op for coroutine 0: sets timer 2 ($1E(a4)) to the word operand, saves the next op in slot 0 ($38(a4)) and returns to the code that stepped the coroutine
; In:  a2 = coroutine script: word ticks; (sp) = the stepping script's a2
; Out: a2 = popped caller script, NEXT (as the return op $2D2A)
; Evidence: coroutine slots $38/$3C/$40/$44(a4) are started by $2334-$240C (store a script address) and stepped by $235A/$23A2/$23EA/$2432 (push a2, a2 = slot, NEXT); $2D28 is an empty coroutine (its first op is the return $2D2A); the next op is typically Op2_WaitTimer2; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer2Yield0:
	move.w	(a2)+,$1E(a4)
	move.l	a2,$38(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
