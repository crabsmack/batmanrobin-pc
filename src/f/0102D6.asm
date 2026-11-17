; ---------------------------------------------------------------------------
; Player_Despawn - takes a player out of play: handler back to Player_JoinOrSpawn, record in-play bit cleared, sprite frames and collision boxes released and the 3 pool blocks freed
; In:  a6 = player object, a2 = record
; Out: record +$25 bit 0 clear, +$3A/+$36/+$38 = 0
; Evidence: move.l #$101CE,(a6); Obj_ReleaseFrames, Obj_SetBodyBoxes/Obj_SetAttackBoxes with d7 = 0, ObjPool_Free x3; only from Player_DespawnNative; continue / game-over scenarios
; ---------------------------------------------------------------------------
Player_Despawn:
	move.l	#$101CE,(a6)
	bclr	#$0,$25(a2)
	sf.b	$3A(a2)
	clr.w	$36(a2)
	clr.w	$38(a2)
	movea.w	$8(a6),a3
	movea.w	$A(a6),a4
	movea.w	$C(a6),a5
	jsr	(Obj_ReleaseFrames).l
	moveq	#$0,d7
	jsr	(Obj_SetBodyBoxes).l
	moveq	#$0,d7
	jsr	(Obj_SetAttackBoxes).l
	movea.l	a3,a0
	bsr.w	ObjPool_Free
	movea.l	a4,a0
	bsr.w	ObjPool_Free
	movea.l	a5,a0
	bra.w	ObjPool_Free
; ---------------------------------------------------------------------------
; Player_Run - handler of a player on screen: feeds the pad into script variable 0 (pressed << 8 | held) and button releases into variable 1 bit $20, runs the player's a2 script, copies its position to the record and adds the weapon charge for its type and level
; In:  a6 = player object (+$58 = 1/2 pad, +$1E = record), a4 = script block
; Out: record +$2E/+$32 = position, +$3A/+$36 from var 1 bit $40, +$38 cleared, +8 += Tbl_WeaponChargeRate[type][level + 1] ($80-$B8), or entry 0 ($280) while the +$26 countdown runs
; Evidence: skips the pad copy while var 1 bit $400; releases tested with $70 (A/B/C) when var 1 bits 1-2 (the flying mode copied from g_PlayMode) else $60; jsr VM2_RunObject; handler long $10322 stored by Player_JoinOrSpawn; 151 corpus runs
; ---------------------------------------------------------------------------
Player_Run:
	movea.w	$A(a6),a4
	move.w	$24(a4),d0
	andi.w	#$400,d0
	bne.s	Player_RunScript
	lea.l	(g_Pad1).w,a0
	cmpi.w	#$1,$58(a6)
	beq.s	loc_010340
	lea.l	(g_Pad2).w,a0
loc_010340:
	move.b	$2(a0),d0
	lsl.w	#$8,d0
	move.b	(a0),d0
	move.w	d0,$22(a4)
	move.b	$3(a0),d0
	move.w	$24(a4),d1
	andi.w	#$6,d1
	beq.s	loc_010362
	andi.w	#$70,d0
	beq.s	Player_RunScript
	bra.s	loc_010368
loc_010362:
	andi.w	#$60,d0
	beq.s	Player_RunScript
loc_010368:
	ori.w	#$20,$24(a4)
; ---------------------------------------------------------------------------
; Player_RunScript - the player's frame after the pad feed: runs its a2 script, copies the position to the record, latches the guard flag ($3A/$36 from var 1 bit $40) and adds the weapon charge
; In:  a6 = player object, a4 = its script block
; Out: as Player_Run
; Evidence: bra from Player_JoinOrSpawn right after the spawn and from Player_Run when var 1 bit $400 skips the pad copy, or after it
; ---------------------------------------------------------------------------
Player_RunScript:
	jsr	(VM2_RunObject).l
	movea.l	$1E(a6),a0
	move.l	$3E(a6),$2E(a0)
	move.l	$42(a6),$32(a0)
	move.w	$24(a4),d0
	andi.w	#$40,d0
	bne.s	loc_010398
	sf.b	$3A(a0)
	clr.w	$36(a0)
	bra.s	loc_0103A2
loc_010398:
	st.b	$3A(a0)
	move.w	$38(a0),$36(a0)
loc_0103A2:
	clr.w	$38(a0)
	tst.w	$26(a0)
	beq.s	loc_0103B4
	subq.w	#$1,$26(a0)
	moveq	#$0,d0
	bra.s	loc_0103BC
loc_0103B4:
	move.w	$2A(a0),d0
	addq.w	#$1,d0
	add.w	d0,d0
loc_0103BC:
	move.w	$28(a0),d1
	asl.w	#$4,d1
	add.w	d1,d0
	move.w	Tbl_WeaponChargeRate(pc,d0.w),d0
	add.w	d0,$8(a0)
	rts
