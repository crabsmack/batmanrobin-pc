; ---------------------------------------------------------------------------
; Op2_BranchNoWall - a2 op: branches when the last move hit no wall ($52(a3) = 0)
; In:  a2 = script: word branch offset; a3 = motion block
; Out: branch or Op2_SkipWord
; Evidence: tst.w $52(a3), bne Op2_SkipWord; 8 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchNoWall:
	tst.w	$52(a3)
	bne.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
