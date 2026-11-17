; ---------------------------------------------------------------------------
; Stage2_RunLift - section $2100: the stage 2 zeppelin lift and the arena boss: music $C, scene Scr6_032E0E, floor y $C4, palettes, a6 loader Scr6_032E4A, plane A at map y $1910, the y-trigger table Tbl_02105A, the lift V-int scroll, camera lock, Stage2_LiftStart and both players' health refilled ($100), then the frame loop (Stage2_LiftUpdate, Trigger_RunY, objects, Stage2_UpdateScroll)
; In:  -
; Out: returns to the section dispatcher when the scene ends (through SceneCb_WaitFadeExit, inferred from the scene pattern)
; Evidence: Tbl_00A0AC entry $2100 (section dispatch); g_Player1/2 +4 = $F654/$F694 health; TAS f21125 (stage 2 start), level-skip scenario f4080
; ---------------------------------------------------------------------------
Stage2_RunLift:
	move.w	#$C,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Scr6_032E0E(pc),a6
	jsr	(Scene_InitStage).l
	move.w	#$C4,(g_FloorY).w
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Scr6_032DAE(pc),a0
	jsr	(Pal_LoadLine3).l
	lea.l	Scr6_032E4A(pc),a6
	jsr	(VM6_Run).l
	lea.l	(g_LayerA).l,a6
	move.w	#$1910,d0
	move.w	d0,(g_MapScrollY).w
	jsr	(Layer_MoveY).l
	move.w	#$FFF0,d0
	jsr	(Layer_MoveX).l
	jsr	(Layer_DrawScreen).l
	lea.l	(Layer_FlushTileQueueIfIdle).l,a0
	jsr	(VIntObj_New).l
	lea.l	(Tbl_02105A).l,a0
	jsr	(Trigger_LoadY).l
	jsr	(Stage2_LiftInstallVInt).l
	jsr	(Camera_LockHere).l
	jsr	(Stage2_LiftStart).l
	move.w	#$100,(g_Player1+$4).w
	move.w	#$100,(g_Player2+$4).w
loc_032FA8:
	jsr	(Frame_BeginStage).l
	tst.b	(g_FreeCam).w
	beq.s	loc_032FBC
	jsr	(Debug_FreeCamStep).l
	bra.s	loc_032FD4
loc_032FBC:
	jsr	(Stage2_LiftUpdate).l
	jsr	(Trigger_RunY).l
	jsr	(Obj_RunList).l
	jsr	(Camera_TrackPlayersX).l
loc_032FD4:
	jsr	(Stage2_UpdateScroll).l
	jsr	(Frame_End).l
	bra.s	loc_032FA8
