; ---------------------------------------------------------------------------
; Op2_DecVar0Loop - a2 op: loop counter: decrements script variable 0 ($22(a4)) and branches while it is not zero
; In:  a2 = script: word branch offset; a4 = object's a4 block
; Out: branch ($2D22) while nonzero, else NEXT past the offset
; Evidence: subq.w then bne $2D22 (the taken-branch tail adda.w (a2),a2); 28 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_DecVar0Loop:
	subq.w	#$1,$22(a4)
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchVar0Any - a2 op: branches when script variable 0 ($22(a4)) has any of the mask's bits
; In:  a2 = script: word mask, word branch offset; a4 = object's a4 block
; Out: branch or NEXT past the offset
; Evidence: and.w (a2)+ then bne $2D22; 153 corpus runs incl. the TAS (the players test their pad bits this way)
; ---------------------------------------------------------------------------
Op2_BranchVar0Any:
	move.w	$22(a4),d0
	and.w	(a2)+,d0
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchVar0None - a2 op: branches when script variable 0 ($22(a4)) has none of the mask's bits
; In:  a2 = script: word mask, word branch offset; a4 = object's a4 block
; Out: branch or NEXT past the offset
; Evidence: and.w (a2)+ then beq $2D22; 147 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchVar0None:
	move.w	$22(a4),d0
	and.w	(a2)+,d0
	beq.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchVar0Eq - a2 op: branches when script variable 0 ($22(a4)) equals a word
; In:  a2 = script: word value, word branch offset; a4 = object's a4 block
; Out: branch or NEXT past the offset
; Evidence: cmp.w (a2)+ then beq $2D22; 6 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchVar0Eq:
	move.w	$22(a4),d0
	cmp.w	(a2)+,d0
	beq.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
