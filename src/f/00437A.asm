; ---------------------------------------------------------------------------
; Op2_WaitPopBelow - a2 op: waits (yielding, retried every frame) until g_ObjPopulation is below the operand, then continues
; In:  a2 = script: word limit
; Out: NEXT when below; else a2 back on this op and Op2_Yield
; Evidence: cmp.w (a2)+,d0 / blt $7E0 / subq.l #4,a2 / bra Op2_Yield; 3 corpus runs; $18066 `437A 0001`
; ---------------------------------------------------------------------------
Op2_WaitPopBelow:
	move.w	(g_ObjPopulation).w,d0
	cmp.w	(a2)+,d0
	blt.w	VM2_Next
	subq.l	#$4,a2
	bra.w	Op2_Yield
; ---------------------------------------------------------------------------
; Op2_WaitPopBelowYield0 - a2 op for coroutine 0: as Op2_WaitPopBelow, but waits by saving this op in slot 0 ($38(a4)) and returning to the stepping script
; In:  a2 = coroutine script: word limit; (sp) = the stepping script's a2
; Out: NEXT when below; else slot 0 = this op, a2 popped, NEXT
; Evidence: subq.l #4,a2 / move.l a2,$38(a4) / movea.l (a7)+,a2; 17 corpus runs; $36A3C `438A 000F`
; ---------------------------------------------------------------------------
Op2_WaitPopBelowYield0:
	move.w	(g_ObjPopulation).w,d0
	cmp.w	(a2)+,d0
	blt.w	VM2_Next
	subq.l	#$4,a2
	move.l	a2,$38(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_WaitPopBelowYield1 - a2 op for coroutine 1: as Op2_WaitPopBelowYield0 with slot 1 ($3C(a4))
; In:  a2 = coroutine script: word limit; (sp) = the stepping script's a2
; Out: NEXT when below; else slot 1 = this op, a2 popped, NEXT
; Evidence: move.l a2,$3C(a4); 5 corpus runs; $395F4 `43A0 0004`
; ---------------------------------------------------------------------------
Op2_WaitPopBelowYield1:
	move.w	(g_ObjPopulation).w,d0
	cmp.w	(a2)+,d0
	blt.w	VM2_Next
	subq.l	#$4,a2
	move.l	a2,$3C(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
