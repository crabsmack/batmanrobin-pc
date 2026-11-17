; ---------------------------------------------------------------------------
; Op2_StartCoroutine3Here - a2 op: points coroutine slot 3 ($44(a4)) at the next op and continues there
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: move.l a2,$44(a4); dispatched in 5 runs (TAS from f20399)
; ---------------------------------------------------------------------------
Op2_StartCoroutine3Here:
	move.l	a2,$44(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StartCoroutine3Rel - a2 op: starts coroutine slot 3 ($44(a4)) at a relative script address
; In:  a2 = script: word offset
; Out: NEXT after the operand
; Evidence: adda.w (a2)+ then move.l a0,$44(a4); dispatched in 50 runs; the absolute twin $2420 never ran
; ---------------------------------------------------------------------------
Op2_StartCoroutine3Rel:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	move.l	a0,$44(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
