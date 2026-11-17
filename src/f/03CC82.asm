; ---------------------------------------------------------------------------
; Stage3_RevealInit - starts the tile-by-tile reveal of the clearing's plane-B picture: clears the 40x22-word cell map g_SceneWorkBuf ($6E0 bytes), cursor g_SceneVars+0 = the map, +2/+4 = column -1 / row 0, +6 = a random spot-order seed, and makes the V-int object Stage3_RevealVIntFlush (its a6 in +8)
; In:  -
; Out: d0 = 0, or nonzero when no pool block was free for the V-int object
; Evidence: Op2_CallNativeRel at $03CBB8 in the director Scr2_03CB90; Mem_Clear $6E0, Rand_Next, VIntObj_New(loc Stage3_RevealVIntFlush); TAS f97888 (once)
; ---------------------------------------------------------------------------
Stage3_RevealInit:
	move.w	#$FFFF,(g_SceneVars+$2).w
	clr.w	(g_SceneVars+$4).w
	lea.l	(g_SceneWorkBuf).l,a0
	move.w	a0,(g_SceneVars).w
	move.w	#$6E0,d7
	jsr	(Mem_Clear).l
	jsr	(Rand_Next).l
	move.w	d0,(g_SceneVars+$6).w
	lea.l	(Stage3_RevealVIntFlush).l,a0
	jsr	(VIntObj_New).l
	bne.s	loc_03CCBE
	move.w	a6,(g_SceneVars+$8).w
	moveq	#$0,d0
loc_03CCBE:
	rts
