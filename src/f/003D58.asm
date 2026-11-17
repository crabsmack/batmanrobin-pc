; ---------------------------------------------------------------------------
; Op2_BranchStatGE - shared tail of the player-stat branch ops: branches when d0 >= the word operand; not dispatched as an op on its own
; In:  d0 = the stat word (Op2_BranchWeaponLvlMaxGE and the never-run stat stubs $3CDC-$3D54); a2 = script: word value, word offset
; Out: branch (Op2_Goto), else NEXT after the operands
; Evidence: cmp.w (a2)+,d0 / bge Op2_Goto; reached by bra from $3CEE (42 corpus runs) and the stubs before it
; ---------------------------------------------------------------------------
Op2_BranchStatGE:
	cmp.w	(a2)+,d0
	bge.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
