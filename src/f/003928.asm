; ---------------------------------------------------------------------------
; Op2_BranchBoxStruck - a2 op: branches when the object's body box number i was struck (Obj_TestBoxStruck)
; In:  a2 = script: word box index, word offset
; Out: branch when struck (g_StruckBox = it), else NEXT after the offset
; Evidence: bsr Obj_TestBoxStruck / bne Op2_Goto; 3 runs
; ---------------------------------------------------------------------------
Op2_BranchBoxStruck:
	bsr.w	Obj_TestBoxStruck
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchBoxNotStruck - a2 op: branches when the object's body box number i was not struck
; In:  a2 = script: word box index, word offset
; Out: branch when not struck, else NEXT after the offset
; Evidence: bsr Obj_TestBoxStruck / beq Op2_Goto; 5 runs
; ---------------------------------------------------------------------------
Op2_BranchBoxNotStruck:
	bsr.w	Obj_TestBoxStruck
	beq.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
