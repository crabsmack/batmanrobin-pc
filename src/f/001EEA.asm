; ---------------------------------------------------------------------------
; Op2_BranchFloorMask - a2 op: branches when the last floor contact result $54(a3) shares a bit with the mask word (landed on that kind of floor)
; In:  a2 = script: word mask, word branch offset; a3 = motion block
; Out: branch or Op2_SkipWord
; Evidence: and.w $54(a3); $54 = d7 of Coll_MoveScene (floor result, e.g. 1 street / 2 or 4 walkway in Coll_Stage1StreetFloors); 94 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchFloorMask:
	move.w	(a2)+,d0
	and.w	$54(a3),d0
	beq.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchOnFloor - a2 op: branches when the object touched any floor on its last move ($54(a3) nonzero)
; In:  a2 = script: word branch offset; a3 = motion block
; Out: branch or Op2_SkipWord
; Evidence: tst.w $54(a3); 14 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchOnFloor:
	tst.w	$54(a3)
	beq.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
