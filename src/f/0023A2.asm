; ---------------------------------------------------------------------------
; Op2_StepCoroutine1 - a2 op: runs coroutine slot 1 ($3C(a4)) for one step (pushes a2, continues at the slot; back through the return op $2D2A)
; In:  a2 = script (no operand); $3C(a4) = coroutine script
; Out: a2 = slot script, (sp) = caller a2; NEXT
; Evidence: move.l a2,-(a7) / movea.l $3C(a4),a2; dispatched in 131 runs, 98k TAS frames; $2398 (never run) would call the slot as native code
; ---------------------------------------------------------------------------
Op2_StepCoroutine1:
	move.l	a2,-(a7)
; ---------------------------------------------------------------------------
; Op2_EnterCoroutine1 - a2 op: continues the script at coroutine slot 1 ($3C(a4)) without pushing the current a2 (Op2_StepCoroutine1 past its push): the return op that ends the step pops the a2 an earlier Op2_StepCoroutineN pushed
; In:  a2 = script (no operand); $3C(a4) = coroutine script
; Out: a2 = slot script; NEXT
; Evidence: a computed-jump target in the corpus: script handler words $23A4 dispatched from Op2_SkipWord's NEXT ($7E8), 487,711 call-site log entries over the corpus (`dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_EnterCoroutine1:
	movea.l	$3C(a4),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StopCoroutine2 - a2 op: stops coroutine slot 2 by pointing $40(a4) at Scr2_002D28 (just the return op)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: move.l #$2D28,$40(a4); dispatched in 132 runs
; ---------------------------------------------------------------------------
Op2_StopCoroutine2:
	move.l	#$2D28,$40(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
