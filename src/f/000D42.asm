; ---------------------------------------------------------------------------
; Op2_SetTimer2Yield2 - a2 op for coroutine 2: sets timer 2 ($1E(a4)) to the word operand, saves the next op in slot 2 ($40(a4)) and returns to the code that stepped the coroutine
; In:  a2 = coroutine script: word ticks; (sp) = the stepping script's a2
; Out: a2 = popped caller script, NEXT (as the return op $2D2A)
; Evidence: coroutine slots $38/$3C/$40/$44(a4) are started by $2334-$240C (store a script address) and stepped by $235A/$23A2/$23EA/$2432 (push a2, a2 = slot, NEXT); $2D28 is an empty coroutine (its first op is the return $2D2A); the next op is typically Op2_WaitTimer2; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer2Yield2:
	move.w	(a2)+,$1E(a4)
	move.l	a2,$40(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTimer3Yield2 - a2 op for coroutine 2: sets timer 3 ($20(a4)) to the word operand, saves the next op in slot 2 ($40(a4)) and returns to the code that stepped the coroutine
; In:  a2 = coroutine script: word ticks; (sp) = the stepping script's a2
; Out: a2 = popped caller script, NEXT (as the return op $2D2A)
; Evidence: coroutine slots $38/$3C/$40/$44(a4) are started by $2334-$240C (store a script address) and stepped by $235A/$23A2/$23EA/$2432 (push a2, a2 = slot, NEXT); $2D28 is an empty coroutine (its first op is the return $2D2A); the next op is typically Op2_WaitTimer3; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTimer3Yield2:
	move.w	(a2)+,$20(a4)
	move.l	a2,$40(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
