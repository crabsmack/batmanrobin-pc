; ---------------------------------------------------------------------------
; Scene_InitStage - starts a gameplay section: common scene init, then reads the stage header for the movement area, two player-slot objects, both players' HUD and start positions, join offset, play mode, terrain routines and camera start, and fades in
; In:  a6 = section header: the Scene_InitCommon fields, then 4 words move-area x ofs/right, y ofs/bottom; 8 words P1 HUD x/y (+8), P2 HUD x (+$D4)/y (+8), P1 start x/y, P2 start x/y; words $EA0A/$EA0C, g_PlayMode; longs g_TerrainFn, g_TerrainFnAlt; words g_CameraX/Y
; Out: g_MoveAreaOfsX/Y and sizes $DFDC/$DFE4, records +$A/+$C (HUD home), +$2E/+$32 (start offset), +$2C slot object, +$25 bit 1 when already in play; g_FrameCallback = $9D72 (SceneCb_CheckGameOver); HUD_Init; fade in ($A9F6)
; Evidence: 16 section entry points ($1FB3A, $1FC50, $32F14, $3FE94, $475C2 ...: `lea Tbl_x(pc),a6; jsr $99D4`) right after Sound_PlayMusic; both slot objects run $101CE, which spawns the player at camera + (+$2E,+$32) or, for a joining player, camera + ($EA0A,$EA0C); TAS 16 frames f503-f128749
; ---------------------------------------------------------------------------
Scene_InitStage:
	bsr.w	Scene_InitCommon
	movem.w	(a6)+,d0-d3
	move.w	d0,(g_MoveAreaOfsX).w
	sub.w	d0,d1
	move.w	d1,(g_MoveAreaW).w
	move.w	d2,(g_MoveAreaOfsY).w
	sub.w	d2,d3
	move.w	d3,(g_MoveAreaH).w
	move.l	a6,-(a7)
	lea.l	(Player_JoinOrSpawn).l,a0
	bsr.w	Obj_NewClear
	movea.l	a6,a3
	lea.l	(Player_JoinOrSpawn).l,a0
	bsr.w	Obj_NewClear
	movea.l	a6,a4
	movea.l	(a7)+,a6
	movem.w	(a6)+,d0-d7
	addi.w	#$8,d0
	addi.w	#$8,d1
	addi.w	#$D4,d2
	addi.w	#$8,d3
	lea.l	(g_Player1).l,a2
	btst	#$0,$25(a2)
	beq.s	loc_009A34
	bset	#$1,$25(a2)
loc_009A34:
	move.w	d0,$A(a2)
	move.w	d1,$C(a2)
	move.w	d4,$2E(a2)
	move.w	d5,$32(a2)
	move.w	a3,$2C(a2)
	move.l	a2,$1E(a3)
	lea.l	(g_Player2).l,a2
	btst	#$0,$25(a2)
	beq.s	loc_009A60
	bset	#$1,$25(a2)
loc_009A60:
	move.w	d2,$A(a2)
	move.w	d3,$C(a2)
	move.w	d6,$2E(a2)
	move.w	d7,$32(a2)
	move.w	a4,$2C(a2)
	move.l	a2,$1E(a4)
	move.w	(a6)+,(g_JoinOfsX).w
	move.w	(a6)+,(g_JoinOfsY).w
	move.w	(a6)+,(g_PlayMode).w
	move.l	(a6)+,(g_TerrainFn).w
	move.l	(a6)+,(g_TerrainFnAlt).w
	move.w	(a6)+,(g_CameraX).w
	move.w	(a6)+,(g_CameraY).w
	move.l	#$9D72,(g_FrameCallback).w
	bsr.w	HUD_Init
	bra.w	Fade_StartIn
