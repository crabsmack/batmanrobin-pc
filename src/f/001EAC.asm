; ---------------------------------------------------------------------------
; Op2_BranchWallMask - a2 op: branches when the last wall contact result $52(a3) shares a bit with the mask word
; In:  a2 = script: word mask, word branch offset; a3 = motion block
; Out: branch or Op2_SkipWord
; Evidence: and.w $52(a3); $52 = d6 of Coll_MoveScene (wall result) stored by $4CDA; 16 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchWallMask:
	move.w	(a2)+,d0
	and.w	$52(a3),d0
	beq.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
