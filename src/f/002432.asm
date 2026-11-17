; ---------------------------------------------------------------------------
; Op2_StepCoroutine3 - a2 op: runs coroutine slot 3 ($44(a4)) for one step (pushes a2, continues at the slot)
; In:  a2 = script (no operand); $44(a4) = coroutine script
; Out: a2 = slot script, (sp) = caller a2; NEXT
; Evidence: move.l a2,-(a7) / movea.l $44(a4),a2; dispatched in 57 runs; slot 3 holds the SetTimer3..Yield3 / WaitTimer3 coroutines
; ---------------------------------------------------------------------------
Op2_StepCoroutine3:
	move.l	a2,-(a7)
	movea.l	$44(a4),a2
	movea.w	(a2)+,a0
	jmp	(a0)
