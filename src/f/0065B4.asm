; ---------------------------------------------------------------------------
; Op4_BranchTouchPlayer - a4 op: branches when a player box overlaps the box d4 +- w by d5 +- h around the drawn point (Hitbox_FindPlayerBox); otherwise continues past the offset
; In:  a4 = script: word half-width, word half-height, word branch offset; d4/d5 = drawn x/y words
; Out: NEXT or the branch (a2 = the box found); d4/d5 kept
; Evidence: bsr Hitbox_FindPlayerBox / bne Op4_Goto; script uses $1323C (`65B4 0004 0004 0004`, health pickup: touched -> collect), $1ABA2; 86 corpus runs
; ---------------------------------------------------------------------------
Op4_BranchTouchPlayer:
	move.w	d4,d2
	move.w	d5,d3
	move.w	(a4)+,d6
	move.w	(a4)+,d7
	sub.w	d6,d4
	add.w	d2,d6
	sub.w	d7,d5
	add.w	d3,d7
	bsr.w	Hitbox_FindPlayerBox
	bne.w	Op4_Goto
	move.w	d2,d4
	move.w	d3,d5
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_HitPlayersOnPhase - a4 op: Op4_HitPlayers on the object's own frame phase only (g_FrameParity == $18(a6)); on the other frames skips its 10 operand bytes
; In:  a4 = script: as Op4_HitPlayers; $18(a6) = phase from Op4_TakeHitPhaseB
; Out: NEXT or the branch
; Evidence: cmp.b $18(a6) with g_FrameParity / lea $A(a4),a4; enemy bullet scripts $42984 (`65D6 0005 0005 0004 29BC 0004`), $42F6A, $46B68; 10 corpus runs
; ---------------------------------------------------------------------------
Op4_HitPlayersOnPhase:
	move.b	(g_FrameParity).w,d0
	cmp.b	$18(a6),d0
	beq.s	Op4_HitPlayers
	lea.l	$A(a4),a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_HitPlayers - a4 op: calls a native hit routine for every player box overlapping d4 +- w by d5 +- h (a2 = the box), then branches; with no overlap continues past the offset
; In:  a4 = script: word half-width, word half-height, long callback, word branch offset; d4/d5 = drawn x/y words
; Out: NEXT or the branch after the callbacks; d4/d5 kept
; Evidence: Hitbox_FindPlayerBox, jsr (a3), Hitbox_FindNextBox loop; script uses $15066 (`65E8 0003 0003 0001 06A8 000C`: callback $106A8), $150A4; 103 corpus runs
; ---------------------------------------------------------------------------
Op4_HitPlayers:
	move.w	d4,d2
	move.w	d5,d3
	move.w	(a4)+,d6
	move.w	(a4)+,d7
	sub.w	d6,d4
	add.w	d2,d6
	sub.w	d7,d5
	add.w	d3,d7
	movea.l	(a4)+,a3
	bsr.w	Hitbox_FindPlayerBox
	bne.s	loc_00660A
	move.w	d2,d4
	move.w	d3,d5
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
loc_00660A:
	jsr	(a3)
	bsr.w	Hitbox_FindNextBox
	bne.s	loc_00660A
	move.w	d2,d4
	move.w	d3,d5
	adda.w	(a4),a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_BranchTouchEnemyOnPhase - a4 op: on the object's own frame phase branches when an enemy box overlaps d4 +- w by d5 +- h (Hitbox_FindEnemyBox); otherwise (or on the other frames) continues past the operands
; In:  a4 = script: word half-width, word half-height, word branch offset; $18(a6) = phase
; Out: NEXT or the branch (a2 = the box)
; Evidence: g_FrameParity test, skip 6 bytes off-phase; player shot scripts $505A0 (`661C 0008 0008 0048 67D2`), $508DC; 150 corpus runs
; ---------------------------------------------------------------------------
Op4_BranchTouchEnemyOnPhase:
	move.b	(g_FrameParity).w,d0
	cmp.b	$18(a6),d0
	beq.s	loc_00662C
	addq.l	#$6,a4
	movea.w	(a4)+,a0
	jmp	(a0)
loc_00662C:
	move.w	d4,d2
	move.w	d5,d3
	move.w	(a4)+,d6
	move.w	(a4)+,d7
	sub.w	d6,d4
	add.w	d2,d6
	sub.w	d7,d5
	add.w	d3,d7
	bsr.w	Hitbox_FindEnemyBox
	bne.w	Op4_Goto
	move.w	d2,d4
	move.w	d3,d5
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
