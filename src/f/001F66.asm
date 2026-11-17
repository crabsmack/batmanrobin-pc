; ---------------------------------------------------------------------------
; Op2_BranchNotFloorMask - a2 op: branches when the last floor result $54(a3) has none of the mask's bits
; In:  a2 = script: word mask, word branch offset; a3 = motion block
; Out: branch or Op2_SkipWord
; Evidence: and.w $54(a3), bne Op2_SkipWord; 89 corpus runs incl. the TAS (36584 TAS frames: run every frame by the walkers)
; ---------------------------------------------------------------------------
Op2_BranchNotFloorMask:
	move.w	(a2)+,d0
	and.w	$54(a3),d0
	bne.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchOffFloor - a2 op: branches when the object touched no floor on its last move ($54(a3) = 0: airborne)
; In:  a2 = script: word branch offset; a3 = motion block
; Out: branch or Op2_SkipWord
; Evidence: tst.w $54(a3), bne Op2_SkipWord; 20 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchOffFloor:
	tst.w	$54(a3)
	bne.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
