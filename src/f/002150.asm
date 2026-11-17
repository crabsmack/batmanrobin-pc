; ---------------------------------------------------------------------------
; Op2_BranchVar1Lt - a2 op: branches when script variable 1 ($24(a4)) is less than a word (signed)
; In:  a2 = script: word value, word branch offset; a4 = object's a4 block
; Out: branch or NEXT past the offset
; Evidence: cmp.w (a2)+ then blt $2D22; 24 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchVar1Lt:
	move.w	$24(a4),d0
	cmp.w	(a2)+,d0
	blt.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
