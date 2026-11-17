; ---------------------------------------------------------------------------
; Op2_StartCoroutine2Here - a2 op: points slot 2 ($40(a4)) at the next op and continues there
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: move.l a2,$40(a4); dispatched in 96 runs
; ---------------------------------------------------------------------------
Op2_StartCoroutine2Here:
	move.l	a2,$40(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StartCoroutine2Rel - a2 op: sets slot 2 ($40(a4)) to a relative address: a coroutine script for Op2_StepCoroutine2, or native code for Op2_CallSlot2Native
; In:  a2 = script: word offset
; Out: NEXT after the operand
; Evidence: adda.w (a2)+ then move.l a0,$40(a4); dispatched in 148 runs; TAS f11040 the object block $BB64 running that script has slot 2 = $1EA24 (68000 code) while its script loops on Op2_CallSlot2Native
; ---------------------------------------------------------------------------
Op2_StartCoroutine2Rel:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	move.l	a0,$40(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
