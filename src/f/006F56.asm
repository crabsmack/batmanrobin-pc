; ---------------------------------------------------------------------------
; VM5_CloseGroup - closes a wave's kill group at the wave's end: its member target becomes the spawned count, and the block is freed if all of those are already dead
; In:  a6 = wave object ($32 = group block or 0)
; Out: rts
; Evidence: +0 = +6, ObjPool_Free when +8 >= it; called by VM5_EndWave
; ---------------------------------------------------------------------------
VM5_CloseGroup:
	move.w	$32(a6),d0
	beq.s	loc_006F6E
	movea.w	d0,a0
	move.w	$6(a0),d0
	move.w	d0,(a0)
	cmp.w	$8(a0),d0
	bgt.s	loc_006F6E
	bsr.w	ObjPool_Free
loc_006F6E:
	rts
; ---------------------------------------------------------------------------
; SpawnAction_GroupMemberReward - spawn action 1 (a kill-group member died): counts the group's kills left down and, at 0 before the deadline (+$A, 0 = none), runs the group's reward action at the dead enemy's position; then counts the death (+8) and frees the group when all members are dead
; In:  d1 = group block (the dead object's $1A, linked by the spawn routine $6FEA); a6 = dead object
; Out: rts
; Evidence: Tbl_012EF0 entry 1; subq.w #1,2(a0), g_FrameCounter vs +$A, SpawnAction_Run(+4); falls into the SpawnAction_GroupMemberGone tail; 22 corpus runs
; ---------------------------------------------------------------------------
SpawnAction_GroupMemberReward:
	tst.w	d1
	bne.s	loc_006F76
	rts
loc_006F76:
	movea.w	d1,a0
	subq.w	#$1,$2(a0)
	bne.s	SpawnAction_GroupCountGone
	move.l	$A(a0),d0
	beq.s	loc_006F8A
	cmp.l	(g_FrameCounter).w,d0
	bcs.s	SpawnAction_GroupCountGone
loc_006F8A:
	move.w	$4(a0),d0
	move.l	a0,-(a7)
	jsr	(SpawnAction_Run).l
	movea.l	(a7)+,a0
	bra.s	SpawnAction_GroupCountGone
