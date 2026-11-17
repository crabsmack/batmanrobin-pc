; ---------------------------------------------------------------------------
; Op2_BranchVar2Any - a2 op: branches when script variable 2 ($26(a4)) has any of the mask's bits
; In:  a2 = script: word mask, word branch offset; a4 = object's a4 block
; Out: branch or NEXT past the offset
; Evidence: and.w (a2)+ then bne $2D22; 19 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchVar2Any:
	move.w	$26(a4),d0
	and.w	(a2)+,d0
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
