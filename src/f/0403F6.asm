; ---------------------------------------------------------------------------
; Stage3_RunCardHall - section $3400 routine, the stage 3 card hall: music $A, scene Tbl_04036E, flat floor at y $9C, palettes, a6 loader Tbl_0403AA, triggers Tbl_034142; frame loop runs the world with layer A, Stage3_CardHallFloorScroll, then the parallax bands Tbl_00978E
; In:  -
; Out: never returns (Scene_FadeOutExit)
; Evidence: Tbl_00A0AC entry $3400 -> $0403F6; TAS f108293 (one frame), level-skip f10311; png f108600: card portraits (Batman, the Mad Hatter, a rabbit) above a checkerboard floor with posts in front
; ---------------------------------------------------------------------------
Stage3_RunCardHall:
	move.w	#$A,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Tbl_04036E(pc),a6
	jsr	(Scene_InitStage).l
	move.w	#$9C,(g_FloorY).w
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_03FDE8(pc),a0
	jsr	(Pal_LoadLine3).l
	lea.l	Tbl_0403AA(pc),a6
	jsr	(VM6_Run).l
	lea.l	(Tbl_034142).l,a0
	jsr	(Trigger_LoadX).l
loc_04043C:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorldLayerA).l
	bsr.w	Stage3_CardHallFloorScroll
	lea.l	(Tbl_00978E).l,a5
	jsr	(Scroll_RunParallax).l
	jsr	(Frame_End).l
	bra.s	loc_04043C
