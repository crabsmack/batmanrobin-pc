; ---------------------------------------------------------------------------
; Op2_StartCoroutine0Here - a2 op: points coroutine slot 0 ($38(a4)) at the next op and continues there, so the ops that follow run now and again on every Op2_StepCoroutine0 (a restart point for a coroutine body that ends in the return op)
; In:  a2 = script (no operand); a4 = object's a4 block
; Out: NEXT
; Evidence: move.l a2,$38(a4); dispatched in all 153 corpus runs; e.g. $1B040 `2334 ... 2D2A` (body ends in the return op $2D2A)
; ---------------------------------------------------------------------------
Op2_StartCoroutine0Here:
	move.l	a2,$38(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StartCoroutine0Rel - a2 op: starts coroutine slot 0 ($38(a4)) at a relative script address (operand address + offset); the slot is then run a step at a time by Op2_StepCoroutine0
; In:  a2 = script: word offset (from the offset word)
; Out: NEXT after the operand
; Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,$38(a4); dispatched in 153 runs; $1E79C `233C 0032` in a stage 1 script that runs at TAS f10512-12924 (f11400 shows the big robot)
; ---------------------------------------------------------------------------
Op2_StartCoroutine0Rel:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	move.l	a0,$38(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StartCoroutine0Abs - a2 op: starts coroutine slot 0 ($38(a4)) at an absolute script address
; In:  a2 = script: long address
; Out: NEXT after the operand
; Evidence: move.l (a2)+,$38(a4); dispatched in 31 runs incl. the TAS (e.g. $1B9C8 `2348 0001B9DC`)
; ---------------------------------------------------------------------------
Op2_StartCoroutine0Abs:
	move.l	(a2)+,$38(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
