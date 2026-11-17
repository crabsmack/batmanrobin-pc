; ---------------------------------------------------------------------------
; Stage4_RunTube - section $4400 (the jetpack flight through a ring tunnel): music 7, stage init, palettes, scene art, Camera_LockHere, triggers Tbl_041124, scroll mode $10 (interleaved A/B line pairs), blue tint +2 on lines 0 and 3; frame loop with Stage4_TubeScroll
; In:  none
; Out: returns through the scene-exit path
; Evidence: Tbl_00A0AC maps $4400 to $0479EE; TAS f122651-128749 (png f124200, f127800)
; ---------------------------------------------------------------------------
Stage4_RunTube:
	move.w	#$7,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Tbl_04795A(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_047520(pc),a0
	jsr	(Pal_LoadLine3).l
	lea.l	Tbl_047996(pc),a6
	jsr	(VM6_Run).l
	jsr	(Camera_LockHere).l
	lea.l	Tbl_041124(pc),a0
	jsr	(Trigger_LoadX).l
	jsr	(Camera_UpdateView).l
	move.w	#$10,(g_ScrollMode).w
	move.b	#$2,(g_Pal0TintB).l
	move.b	#$2,(g_Pal3TintB).l
loc_047A4E:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorld).l
	bsr.w	Stage4_TubeScroll
	jsr	(Frame_End).l
	bra.s	loc_047A4E
