; ---------------------------------------------------------------------------
; Op2_DecVar1Loop - a2 op: loop counter on script variable 1: decrements $24(a4) and branches while it is not zero
; In:  a2 = script: word branch offset; a4 = object's a4 block
; Out: branch while nonzero, else NEXT past the offset
; Evidence: subq.w then bne $2D22; 28 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_DecVar1Loop:
	subq.w	#$1,$24(a4)
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchVar1Any - a2 op: branches when script variable 1 ($24(a4)) has any of the mask's bits
; In:  a2 = script: word mask, word branch offset; a4 = object's a4 block
; Out: branch or NEXT past the offset
; Evidence: and.w (a2)+ then bne $2D22; 151 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchVar1Any:
	move.w	$24(a4),d0
	and.w	(a2)+,d0
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchVar1None - a2 op: branches when script variable 1 ($24(a4)) has none of the mask's bits
; In:  a2 = script: word mask, word branch offset; a4 = object's a4 block
; Out: branch or NEXT past the offset
; Evidence: and.w (a2)+ then beq $2D22; 103 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchVar1None:
	move.w	$24(a4),d0
	and.w	(a2)+,d0
	beq.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchVar1Eq - a2 op: branches when script variable 1 ($24(a4)) equals a word
; In:  a2 = script: word value, word branch offset; a4 = object's a4 block
; Out: branch or NEXT past the offset
; Evidence: cmp.w (a2)+ then beq $2D22; 5 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchVar1Eq:
	move.w	$24(a4),d0
	cmp.w	(a2)+,d0
	beq.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
