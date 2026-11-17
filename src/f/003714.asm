; ---------------------------------------------------------------------------
; Op2_BranchTouchBox - a2 op: branches when any box on a collision list overlaps a rectangle relative to the object (mirrored by its H/V flip bits), remembering that box in g_FoundBox; the kind word picks the list through a jump table (0/4 = g_PlayerBoxList, 8 = g_EnemyBoxList)
; In:  a2 = script: word kind (0, 4 or 8), words left, top, right, bottom (relative to $3E/$42(a6)), word offset
; Out: found: g_FoundBox = box, branch; else NEXT after the operands
; Evidence: movem.w (a0)+,d3-d7 / neg+exg on bits 3/4 of $38(a5) / jmp $374A(pc,d3.w) (bra $375A, $375A, $3768); 148 runs; $132FE `3714 0000 FFFE FFFE 0002 0002 005A`; g_FoundBox is then used by native callbacks ($10742 damages it)
; ---------------------------------------------------------------------------
Op2_BranchTouchBox:
	move.w	$3E(a6),d1
	move.w	$42(a6),d2
	movea.l	a2,a0
	movem.w	(a0)+,d3-d7
	btst	#$3,$38(a5)
	beq.s	loc_003730
	neg.w	d4
	neg.w	d6
	exg.l	d4,d6
loc_003730:
	btst	#$4,$38(a5)
	beq.s	loc_00373E
	neg.w	d5
	neg.w	d7
	exg.l	d5,d7
loc_00373E:
	add.w	d1,d4
	add.w	d1,d6
	add.w	d2,d5
	add.w	d2,d7
	jmp	Op2_BranchTouchBox_Kind0(pc,d3.w)
; ---------------------------------------------------------------------------
; Op2_BranchTouchBox_Kind0 - Op2_BranchTouchBox's jump-table slot for kind 0: on to the player-list case
; In:  d4-d7 = the rectangle; a0 = the op's offset word
; Out: bra.w Op2_BranchTouchBox_Players
; Evidence: the jmp $374A(pc,d3.w) at $3746 with d3 = 0: 751,562 call-site log entries over the corpus
; ---------------------------------------------------------------------------
Op2_BranchTouchBox_Kind0:
	bra.w	Op2_BranchTouchBox_Players
; ---------------------------------------------------------------------------
; Op2_BranchTouchBox_Kind4 - Op2_BranchTouchBox's jump-table slot for kind 4: also the player-list case
; In:  d4-d7 = the rectangle; a0 = the op's offset word
; Out: bra.w Op2_BranchTouchBox_Players
; Evidence: the jmp $374A(pc,d3.w) at $3746 with d3 = 4: 707,192 call-site log entries over the corpus
; ---------------------------------------------------------------------------
Op2_BranchTouchBox_Kind4:
	bra.w	Op2_BranchTouchBox_Players
; ---------------------------------------------------------------------------
; Op2_BranchTouchBox_Kind8 - Op2_BranchTouchBox's jump-table slot for kind 8: the enemy-list case
; In:  d4-d7 = the rectangle; a0 = the op's offset word
; Out: bra.w Op2_BranchTouchBox_Enemies
; Evidence: the jmp $374A(pc,d3.w) at $3746 with d3 = 8: 18,686 call-site log entries over the corpus
; ---------------------------------------------------------------------------
Op2_BranchTouchBox_Kind8:
	bra.w	Op2_BranchTouchBox_Enemies
