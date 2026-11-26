; ---------------------------------------------------------------------------
; Op2_BranchVar1Ne - a2 op: branches when script variable 1 ($24(a4)) differs from a word
; In:  a2 = script: word value, word branch offset; a4 = object's a4 block
; Out: branch (Op2_Goto) or NEXT past the offset
; Evidence: cmp.w (a2)+ then bne Op2_Goto, Op2_BranchVar1Eq with bne; never run in the corpus; named at a script op position no corpus run read (tools/raw_reach.py, R27)
; ---------------------------------------------------------------------------
Op2_BranchVar1Ne:
	move.w	$24(a4),d0
	cmp.w	(a2)+,d0
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
