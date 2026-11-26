; ---------------------------------------------------------------------------
; Op2_BranchTouchBox_KindC - Op2_BranchTouchBox's jump-table slot for kind $C: the enemy-list case (bra.w Op2_BranchTouchBox_Enemies)
; In:  as Op2_BranchTouchBox
; Out: as Op2_BranchTouchBox_Enemies
; Evidence: entry $C of the jmp $374A(pc,d3.w) table, after Kind8 (the same branch); never run in the corpus (converted 2026-11-17 after a playtest stopped on $13060)
; ---------------------------------------------------------------------------
Op2_BranchTouchBox_KindC:
	bra.w	Op2_BranchTouchBox_Enemies
