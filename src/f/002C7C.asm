; ---------------------------------------------------------------------------
; Op2_LoopCounter1 - a2 op: decrements loop counter 1 ($16(a4)) and branches while it is above 0
; In:  a2 = script: word offset
; Out: branch via Op2_Goto while > 0, else NEXT after the offset
; Evidence: subq.w #1,$16(a4) / bgt; 32 runs
; ---------------------------------------------------------------------------
Op2_LoopCounter1:
	subq.w	#$1,$16(a4)
	bgt.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetCounter2 - a2 op: sets loop counter 2 ($18(a4)) for Op2_LoopCounter2
; In:  a2 = script: word count
; Out: NEXT
; Evidence: move.w (a2)+,$18(a4); 15 runs
; ---------------------------------------------------------------------------
Op2_SetCounter2:
	move.w	(a2)+,$18(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
