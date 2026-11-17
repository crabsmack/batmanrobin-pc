; ---------------------------------------------------------------------------
; Stage3_RunHatterRoad - section $3401 routine, the Mad Hatter boss on the checkered road through space: music 6, scene Tbl_0404E4, palette line 0, a6 loader Tbl_040520, triggers Scr5_034932, camera x $A5, palette lines 1 and 2 tinted -2, Stage3_InitHatterRoad; frame loop runs triggers, objects and then Stage3_RunRoadEndSwirl (the road object, not Camera_TrackPlayersX, moves the camera)
; In:  -
; Out: never returns (Scene_FadeOutExit)
; Evidence: Tbl_00A0AC entry $3401 -> $040574; TAS f113271 (one frame), level-skip f11049; png f113400-f115800: a giant top hat with the Hatter on its brim above a bending checkered road, BOSS TIME
; ---------------------------------------------------------------------------
Stage3_RunHatterRoad:
	move.w	#$6,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Tbl_0404E4(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_040520(pc),a6
	jsr	(VM6_Run).l
	lea.l	(Scr5_034932).l,a0
	jsr	(Trigger_LoadX).l
	move.w	#$A5,(g_CameraX).w
	jsr	(Camera_UpdateView).l
	moveq	#-$2,d0
	move.b	d0,(g_Pal1TintR).w
	move.b	d0,(g_Pal1TintG).w
	move.b	d0,(g_Pal1TintB).w
	move.b	d0,(g_Pal2TintR).w
	move.b	d0,(g_Pal2TintG).w
	move.b	d0,(g_Pal2TintB).w
	bsr.w	Stage3_InitHatterRoad
loc_0405D4:
	jsr	(Frame_BeginStage).l
	jsr	(Trigger_RunX).l
	jsr	(Obj_RunList).l
	jsr	Stage3_RunRoadEndSwirl(pc)
	jsr	(Frame_End).l
	bra.s	loc_0405D4
