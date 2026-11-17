; ---------------------------------------------------------------------------
; Player_JoinOrSpawn - handler of a player slot object while no player is on screen: when the record is in play it spawns the player (3 pool blocks, Batman or Robin script, at the saved position or the camera + join offset with $78 frames of invulnerability) and switches to Player_Run; otherwise it waits for Start with a credit to join
; In:  a6 = player slot object (+$1E = record)
; Out: on spawn: handler Player_Run, a3/a4/a5 blocks at +8/+$A/+$C, $58 = record +$3E (1/2), record +$3C = serial; on join: record +$25 bit 0 set, g_Credits - 1, HUD_PlayerJoined
; Evidence: record +$25 bit 0 = in play; join needs g_JoinLocked clear, g_Credits > 0 and Start (bit 7) newly pressed in g_Pad1/2 by +$3E; script Scr2_04ACD0, or Scr2_04D80C when +$25 bit 3 (Robin); +$25 bit 1 = respawn at +$2E/+$32; set as the handler by Scene_InitStage ($99F4/$9A00) and Player_Despawn; every gameplay frame of the corpus (157 runs)
; ---------------------------------------------------------------------------
Player_JoinOrSpawn:
	movea.l	$1E(a6),a0
	btst	#$0,$25(a0)
	beq.s	loc_0101DC
	bne.s	loc_010216
loc_0101DC:
	tst.b	(g_JoinLocked).w
	bne.s	loc_010200
	tst.w	(g_Credits).w
	beq.s	loc_010200
	lea.l	(g_Pad1).w,a1
	cmpi.w	#$1,$3E(a0)
	beq.s	loc_0101F8
	lea.l	(g_Pad2).w,a1
loc_0101F8:
	btst	#$7,$2(a1)
	bne.s	loc_010202
loc_010200:
	rts
loc_010202:
	bset	#$0,$25(a0)
	subq.w	#$1,(g_Credits).w
	bsr.w	HUD_PlayerJoined
	bsr.s	Player_InitRecord
	bra.w	loc_010216
loc_010216:
	move.w	#$3,d0
	bsr.w	ObjPool_HasFree
	beq.w	loc_010224
	rts
loc_010224:
	move.l	#$10322,(a6)
	bsr.w	ObjPool_AllocClear
	movea.l	a0,a3
	bsr.w	ObjPool_AllocClear
	movea.l	a0,a4
	bsr.w	ObjPool_AllocClear
	movea.l	a0,a5
	move.w	a3,$8(a6)
	move.w	a4,$A(a6)
	move.w	a5,$C(a6)
	move.w	#$1,$3A(a5)
	move.w	(g_SceneSprAttr).w,$38(a5)
	move.w	(g_ObjSerial).w,$56(a6)
	addq.w	#$1,(g_ObjSerial).w
	move.w	(g_PlayMode).w,$24(a4)
	movea.l	$1E(a6),a2
	move.w	$3E(a2),$58(a6)
	move.w	$56(a6),$3C(a2)
	move.l	(g_CameraX).w,d0
	move.l	(g_CameraY).w,d1
	swap	d0
	swap	d1
	bclr	#$1,$25(a2)
	beq.s	loc_010292
	add.w	$2E(a2),d0
	add.w	$32(a2),d1
	bra.s	loc_0102A0
loc_010292:
	move.b	#$78,$16(a5)
	add.w	(g_JoinOfsX).w,d0
	add.w	(g_JoinOfsY).w,d1
loc_0102A0:
	swap	d0
	swap	d1
	move.l	d0,$3E(a6)
	move.l	d1,$42(a6)
	lea.l	(Scr2_04ACD0).l,a0
	btst	#$3,$25(a2)
	beq.s	loc_0102C0
	lea.l	(Scr2_04D80C).l,a0
loc_0102C0:
	move.l	a0,$2A(a4)
	bra.w	Player_RunScript
; ---------------------------------------------------------------------------
; Player_DespawnNative - a2 native for the player scripts: removes the player from play (Player_Despawn) and ends the script's frame
; In:  a6 = player object, a2 = script
; Out: does not return to the script: sp = g_VM2SavedSP, rts to the object list
; Evidence: dc.l after op $2CBC in the Batman ($4B1B8, $4D00A ...) and Robin ($4FB46, $4FDBC) scripts, after the lives run out (inferred: Player_LoseLife d0 = 1); 2 runs (scenario-continue, scenario-game-over)
; ---------------------------------------------------------------------------
Player_DespawnNative:
	movea.l	$1E(a6),a2
	bsr.w	Player_Despawn
	movea.l	(g_VM2SavedSP).w,a7
	rts
