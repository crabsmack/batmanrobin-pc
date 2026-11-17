; ---------------------------------------------------------------------------
; Op2_StartCoroutine1Here - a2 op: points coroutine slot 1 ($3C(a4)) at the next op and continues there (restart point for the following coroutine body)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: move.l a2,$3C(a4); dispatched in 83 runs; $1E860 `237C 07FC`: the body can be native code after Op2_Native
; ---------------------------------------------------------------------------
Op2_StartCoroutine1Here:
	move.l	a2,$3C(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StartCoroutine1Rel - a2 op: starts coroutine slot 1 ($3C(a4)) at a relative script address (operand address + offset)
; In:  a2 = script: word offset
; Out: NEXT after the operand
; Evidence: adda.w (a2)+ then move.l a0,$3C(a4); dispatched in 112 runs; the absolute twin $2390 never ran
; ---------------------------------------------------------------------------
Op2_StartCoroutine1Rel:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	move.l	a0,$3C(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
