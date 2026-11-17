; ---------------------------------------------------------------------------
; Boss_Hatter_SetRoadSpeed - a2 native (op $2CBC + 2 words): sets the road speed the steerer eases to: word 0 = target speed (integer, into a4 +$22 as 16.16), word 1 = easing rate per frame in 1/256 (a4 +$24 long; it overwrites the target's fraction word)
; In:  a2 = the two words, a4 = the steerer's a4 block
; Out: a4 +$22/+$24, a2 past the words
; Evidence: dc.l $0003DB50 in Scr2_03DA8C (FFE8 0020, 0000 0008 before Section_StartExitN) and 15 times in Scr2_03F27E; TAS f114542-f115354 (3 frames)
; ---------------------------------------------------------------------------
Boss_Hatter_SetRoadSpeed:
	moveq	#$0,d0
	move.w	(a2)+,d0
	swap	d0
	move.l	d0,$22(a4)
	move.w	(a2)+,d0
	ext.l	d0
	asl.l	#$8,d0
	move.l	d0,$24(a4)
	rts
; ---------------------------------------------------------------------------
; Boss_Hatter_SteerRoad - per-frame road steering: eases the road speed g_SceneVars2+8 (16.16) toward the target a4 +$22 by the rate a4 +$24, and publishes the steerer's world position as the road's bend g_SceneVars2+$16 = x/16 and pitch +$1C = y/16 (Stage3_DrawRoad clamps and draws them)
; In:  a6/a4 = the steerer
; Out: g_SceneVars2+8, +$16, +$1C
; Evidence: dc.l $0003DB66 at $03DAB4 in the steerer's frame loop; asr.l #4 of $3E/$42; TAS f113312-f116145
; ---------------------------------------------------------------------------
Boss_Hatter_SteerRoad:
	move.l	(g_SceneVars2+$8).w,d0
	move.l	$24(a4),d1
	move.l	$22(a4),d2
	cmp.l	d2,d0
	bge.s	loc_03DB80
	add.l	d1,d0
	cmp.l	d2,d0
	ble.s	loc_03DB88
	move.l	d2,d0
	bra.s	loc_03DB88
loc_03DB80:
	sub.l	d1,d0
	cmp.l	d2,d0
	bge.s	loc_03DB88
	move.l	d2,d0
loc_03DB88:
	move.l	d0,(g_SceneVars2+$8).w
	move.l	$3E(a6),d0
	asr.l	#$4,d0
	move.l	d0,(g_SceneVars2+$16).w
	move.l	$42(a6),d0
	asr.l	#$4,d0
	move.l	d0,(g_SceneVars2+$1C).w
	rts
; ---------------------------------------------------------------------------
; Stage3_RunRoadEndSwirl - runs the road-end swirl object (unlinked, its a6 kept in g_SceneVars) inline through VM2_RunObject, after the object list so it is placed on this frame's road rows; nothing when it is 0
; In:  g_SceneVars = the swirl object or 0
; Out: as VM2_RunObject
; Evidence: jsr from Stage3_RunHatterRoad's frame loop after Obj_RunList; ctarget in 6 corpus runs (TAS f113312-f116145)
; ---------------------------------------------------------------------------
Stage3_RunRoadEndSwirl:
	move.w	(g_SceneVars).w,d0
	beq.s	loc_03DBB0
	movea.w	d0,a6
	jmp	(VM2_RunObject).l
loc_03DBB0:
	rts
; ---------------------------------------------------------------------------
; Stage3_SpawnRoadEndSwirl - spawns the swirl at the road's far end, Scr2_03DBC6 (an unlinked enemy-class object: a 12-frame looping animation drawn at the back, placed each frame by Stage3_PlaceRoadEndSwirl), and keeps it in g_SceneVars for Stage3_RunRoadEndSwirl
; In:  -
; Out: g_SceneVars = a6 = the object, d0 = 0 (quits the caller when the pool is full)
; Evidence: jsr from Stage3_InitHatterRoad; Obj_SpawnEnemyUnlinkedOrQuit; Scr2_03DBC6: Op2_SetAnim with 12 Op2_AnimFrame/Op2_DrawSprBack frames $4772-$47E0; png f113400/f114600: a pink swirl where the road begins
; ---------------------------------------------------------------------------
Stage3_SpawnRoadEndSwirl:
	lea.l	(Scr2_03DBC6).l,a2
	jsr	(Obj_SpawnEnemyUnlinkedOrQuit).l
	move.w	a6,(g_SceneVars).w
	moveq	#$0,d0
	rts
