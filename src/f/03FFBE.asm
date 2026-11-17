; ---------------------------------------------------------------------------
; Stage3_RunCatHeadBoss - section $3101 routine, the stage 3 cat-head boss arena: music 4, scene Scr6_03FF2A, flat floor at y $D0, palette line 0, a6 loader Scr6_03FF66, triggers Tbl_033F20, camera locked here; frame loop runs the world then Boss_CatHead_FrameScroll
; In:  -
; Out: never returns (Scene_FadeOutExit)
; Evidence: Tbl_00A0AC entry $3101 -> $03FFBE; TAS f87254 (one frame), level-skip f8097; png f087600/f088800: a giant grinning cat face with claws, BOSS TIME
; ---------------------------------------------------------------------------
Stage3_RunCatHeadBoss:
	move.w	#$4,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Scr6_03FF2A(pc),a6
	jsr	(Scene_InitStage).l
	move.w	#$D0,(g_FloorY).w
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Scr6_03FF66(pc),a6
	jsr	(VM6_Run).l
	lea.l	(Tbl_033F20).l,a0
	jsr	(Trigger_LoadX).l
	jsr	(Camera_LockHere).l
loc_040000:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorld).l
	jsr	Boss_CatHead_FrameScroll(pc)
	jsr	(Frame_End).l
	bra.s	loc_040000
