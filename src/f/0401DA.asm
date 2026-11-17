; ---------------------------------------------------------------------------
; Stage3_RunSpottedRoom - section $3300 routine, the stage 3 red spotted room: music $A, scene Tbl_04017A, palettes, a6 loader Tbl_0401B6, triggers Tbl_03412E, Stage3_SpottedRoomInitScroll, camera locked, full health; frame loop runs triggers, objects, Camera_TrackPlayersX and Stage3_SpottedRoomScroll (the room is plane A drawn in perspective by line and column scroll)
; In:  -
; Out: never returns (Scene_FadeOutExit)
; Evidence: Tbl_00A0AC entry $3300 -> $0401DA; TAS f101674 (one frame), level-skip f9573; png f102000/f105000: a red room of black holes in perspective (ceiling, back wall, floor) that sways
; ---------------------------------------------------------------------------
Stage3_RunSpottedRoom:
	move.w	#$A,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Tbl_04017A(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_03FDE8(pc),a0
	jsr	(Pal_LoadLine3).l
	lea.l	Tbl_0401B6(pc),a6
	jsr	(VM6_Run).l
	lea.l	(Tbl_03412E).l,a0
	jsr	(Trigger_LoadX).l
	jsr	(Stage3_SpottedRoomInitScroll).l
	jsr	(Camera_LockHere).l
	move.w	#$100,(g_Player1+$4).w
	move.w	#$100,(g_Player2+$4).w
loc_040232:
	jsr	(Frame_BeginStage).l
	jsr	(Trigger_RunX).l
	jsr	(Obj_RunList).l
	jsr	(Camera_TrackPlayersX).l
	jsr	(Stage3_SpottedRoomScroll).l
	jsr	(Frame_End).l
	bra.s	loc_040232
