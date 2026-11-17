; ---------------------------------------------------------------------------
; Op2_BrakeXGotoIfStopped - a2 op: sets the x target speed $24(a3) to 0, then branches to its word operand if the x velocity $18(a3) is already 0, else skips the operand and continues
; In:  a2 = script: word branch target; a3 = motion block
; Out: NEXT, or Op2_Goto to the operand
; Evidence: clr.l $24(a3) as Op2_ClearTargetSpeedX, tst.l $18(a3) beq.w Op2_Goto; run only on the stepped runner (P4 kickoff 2026-10-27, 5 fuzz runs)
; ---------------------------------------------------------------------------
Op2_BrakeXGotoIfStopped:
	clr.l	$24(a3)
	tst.l	$18(a3)
	beq.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
