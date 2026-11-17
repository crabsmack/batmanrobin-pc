; ---------------------------------------------------------------------------
; Op2_CallSlot2Native - a2 op: calls slot 2 ($40(a4)) as a native 68000 routine (jsr), then continues the script
; In:  a2 = script (no operand); $40(a4) = native routine
; Out: NEXT (registers as the routine leaves them)
; Evidence: movea.l $40(a4),a0 / jsr (a0); dispatched in 8 runs, TAS f10512-12924 (stage 1 script loop at $1E7B4, slot 2 = $1EA24, which positions the object from g_CameraX); the slot-0/1 twins $2350/$2398 never ran
; ---------------------------------------------------------------------------
Op2_CallSlot2Native:
	movea.l	$40(a4),a0
	jsr	(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StepCoroutine2 - a2 op: runs coroutine slot 2 ($40(a4)) for one step (pushes a2, continues at the slot)
; In:  a2 = script (no operand); $40(a4) = coroutine script
; Out: a2 = slot script, (sp) = caller a2; NEXT
; Evidence: move.l a2,-(a7) / movea.l $40(a4),a2; dispatched in 151 runs
; ---------------------------------------------------------------------------
Op2_StepCoroutine2:
	move.l	a2,-(a7)
; ---------------------------------------------------------------------------
; Op2_EnterCoroutine2 - a2 op: continues the script at coroutine slot 2 ($40(a4)) without pushing the current a2 (Op2_StepCoroutine2 past its push): the return op that ends the step pops the a2 an earlier Op2_StepCoroutineN pushed
; In:  a2 = script (no operand); $40(a4) = coroutine script
; Out: a2 = slot script; NEXT
; Evidence: a computed-jump target in the corpus: script handler words $23EC dispatched from Op2_SetSpeed's NEXT ($B0C), 1,012 call-site log entries over the corpus (`dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_EnterCoroutine2:
	movea.l	$40(a4),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StopCoroutine3 - a2 op: stops coroutine slot 3 by pointing $44(a4) at Scr2_002D28 (just the return op)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: move.l #$2D28,$44(a4); dispatched in 5 runs
; ---------------------------------------------------------------------------
Op2_StopCoroutine3:
	move.l	#$2D28,$44(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
