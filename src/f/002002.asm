; ---------------------------------------------------------------------------
; Op2_IncVar0 - a2 op: adds 1 to script variable 0 ($22(a4))
; In:  a2 = script (no operand); a4 = object's a4 block
; Out: NEXT
; Evidence: addq.w #1,$22(a4); 21 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_IncVar0:
	addq.w	#$1,$22(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DecVar0 - a2 op: subtracts 1 from script variable 0 ($22(a4))
; In:  a2 = script (no operand); a4 = object's a4 block
; Out: NEXT
; Evidence: subq.w #1,$22(a4); 1 corpus run (the TAS)
; ---------------------------------------------------------------------------
Op2_DecVar0:
	subq.w	#$1,$22(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
