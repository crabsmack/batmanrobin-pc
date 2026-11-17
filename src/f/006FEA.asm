; ---------------------------------------------------------------------------
; VM5_SpawnEntry - spawns one enemy from an a5 wave spawn entry: position from the entry (screen-relative or world) plus the wave origin, then the entry's spawner; on success counts the spawn and hands the new object its spawn action (action 1 also joins the wave's open kill group)
; In:  a5 = script at a word offset to the entry (self-relative: word x, word y ($E000-range = screen-relative, + g_CameraX/Y; else world, - $2000), long = spawn action byte + spawner routine), a6 = wave object (+$26/$28 origin, +$32 kill group block or 0)
; Out: a5 past the offset word; d0 = 0 when spawned (+$C spawn count +1; new object +$19 = action; for action 1 +$1A = group, group +6 members +1, +$32 cleared when the member target (group +0) is reached), nonzero when the spawner failed; a6 = wave object (restored)
; Evidence: bsr from Op5_Spawn/Op5_SpawnMulti/Op5_SpawnRandom only; jsr (a0) with d0/a5/a6 saved, the spawner returns the new object in a6; entry $16994 `0154 E018 0001 B6FA` (screen x $154, action 1, spawner $1B6FA); action 1 = SpawnAction_GroupMemberReward; 119 corpus runs, TAS f622-128552
; ---------------------------------------------------------------------------
VM5_SpawnEntry:
	movea.l	a5,a1
	adda.w	(a5)+,a1
	move.w	(a1)+,d4
	move.w	(a1)+,d5
	bmi.s	loc_006FFA
	subi.w	#$2000,d5
	bra.s	loc_007006
loc_006FFA:
	addi.w	#$2000,d5
	add.w	(g_CameraX).w,d4
	add.w	(g_CameraY).w,d5
loc_007006:
	add.w	$26(a6),d4
	add.w	$28(a6),d5
	move.b	(a1),d0
	movea.l	(a1)+,a0
	movem.l	d0/a5-a6,-(a7)
	jsr	(a0)
	movea.l	a6,a0
	movem.l	(a7)+,d0/a5-a6
	bne.s	loc_007050
	addq.w	#$1,$C(a6)
	tst.b	d0
	beq.s	loc_007050
	cmpi.b	#$1,d0
	bne.s	loc_00704A
	move.w	$32(a6),d1
	beq.s	loc_00704E
	move.w	d1,$1A(a0)
	movea.w	d1,a1
	addq.w	#$1,$6(a1)
	move.w	(a1),d1
	cmp.w	$6(a1),d1
	bgt.s	loc_00704A
	clr.w	$32(a6)
loc_00704A:
	move.b	d0,$19(a0)
loc_00704E:
	moveq	#$0,d0
loc_007050:
	rts
; ---------------------------------------------------------------------------
; VM5_AfterSpawn - shared tail of the a5 spawn ops: reloads the wait timer from the interval, switches the script to the after-quota pc when the spawn count reaches the quota, and yields the frame
; In:  a6 = wave object (+$2A interval, +$C spawn count, +$E/$10 quota 1P/2P, +$16 after-quota pc)
; Out: yields (sp from g_VM5SavedSP, rts to VM5_RunObject's caller); $2E = interval, or when the quota is met $1E = +$16 and $2E = 0 (runs again next frame)
; Evidence: branched to by Op5_Spawn/SpawnMulti/SpawnRandom; quota $10(a6) only when bit 0 of g_P1Flags and g_P2Flags are both set (two players), as in VM5_RunObject's cap; movea.l g_VM5SavedSP,a7 / rts
; ---------------------------------------------------------------------------
VM5_AfterSpawn:
	move.l	$2A(a6),$2E(a6)
	move.w	$E(a6),d0
	btst	#$0,(g_P1Flags).w
	beq.s	loc_007070
	btst	#$0,(g_P2Flags).w
	beq.s	loc_007070
	move.w	$10(a6),d0
loc_007070:
	cmp.w	$C(a6),d0
	bgt.s	loc_007080
	move.l	$16(a6),$1E(a6)
	clr.w	$2E(a6)
loc_007080:
	movea.l	(g_VM5SavedSP).w,a7
	rts
