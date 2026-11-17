; ---------------------------------------------------------------------------
; Stage4_RunLab - section $4300 (the lab with machines over a tiled floor): music $B, stage init, floor y $B8, palettes, scene art, triggers Tbl_041000, scroll mode $C (line scroll); frame loop with Stage4_LabScroll
; In:  none
; Out: returns through the scene-exit path
; Evidence: Tbl_00A0AC maps $4300 to $047772; TAS f119385-122651 (png f120000, f122400)
; ---------------------------------------------------------------------------
Stage4_RunLab:
	move.w	#$B,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Tbl_0476EA(pc),a6
	jsr	(Scene_InitStage).l
	move.w	#$B8,(g_FloorY).w
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_047520(pc),a0
	jsr	(Pal_LoadLine3).l
	lea.l	Tbl_047726(pc),a6
	jsr	(VM6_Run).l
	lea.l	Tbl_041000(pc),a0
	jsr	(Trigger_LoadX).l
	move.w	#$C,(g_ScrollMode).w
loc_0477BC:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorld).l
	bsr.w	Stage4_LabScroll
	jsr	(Frame_End).l
	bra.s	loc_0477BC
