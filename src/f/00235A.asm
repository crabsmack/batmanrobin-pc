; ---------------------------------------------------------------------------
; Op2_StepCoroutine0 - a2 op: runs coroutine slot 0 for one step: pushes the current a2 and continues at the slot's script; the coroutine comes back with the return op $2D2A (or a SetTimer..Yield0 op that saves its position first)
; In:  a2 = script (no operand); $38(a4) = coroutine script
; Out: a2 = slot script, (sp) = caller a2; NEXT
; Evidence: move.l a2,-(a7) / movea.l $38(a4),a2; dispatched in 153 runs, 128k TAS frames (once per object per frame)
; ---------------------------------------------------------------------------
Op2_StepCoroutine0:
	move.l	a2,-(a7)
	movea.l	$38(a4),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StopCoroutine1 - a2 op: stops coroutine slot 1 by pointing $3C(a4) at Scr2_002D28 (just the return op)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: move.l #$2D28,$3C(a4); the slot-0 twin is Op2_StopCoroutine0; $2370 (never run) would set it to the native rts $82C instead; dispatched in 78 runs
; ---------------------------------------------------------------------------
Op2_StopCoroutine1:
	move.l	#$2D28,$3C(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
