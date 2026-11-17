; ---------------------------------------------------------------------------
; Stage4_RunHall - section $4000, stage 4's first scene (the hall with arched windows and a rippling blue floor): music $B, stage init, floor y $C8, palettes, scene art, triggers Tbl_040F4A, the cone raster, lines 1/2 tinted -2, both players' health refilled; then the frame loop (parallax command list Tbl_0097AA, cones, floor ripple)
; In:  none
; Out: returns through the scene-exit path
; Evidence: Tbl_00A0AC maps $4000 to $0475C2; TAS f116171-119385 (png f116400-118800)
; ---------------------------------------------------------------------------
Stage4_RunHall:
	move.w	#$B,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Tbl_047540(pc),a6
	jsr	(Scene_InitStage).l
	move.w	#$C8,(g_FloorY).w
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_047520(pc),a0
	jsr	(Pal_LoadLine3).l
	lea.l	Tbl_04757C(pc),a6
	jsr	(VM6_Run).l
	lea.l	Tbl_040F4A(pc),a0
	jsr	(Trigger_LoadX).l
	jsr	(Camera_UpdateView).l
	bsr.w	Stage4_InitConeRaster
	move.b	#$FE,(g_Pal1TintR).l
	move.b	#$FE,(g_Pal1TintG).l
	move.b	#$FE,(g_Pal1TintB).l
	move.b	#$FE,(g_Pal2TintR).l
	move.b	#$FE,(g_Pal2TintG).l
	move.b	#$FE,(g_Pal2TintB).l
	move.w	#$100,(g_Player1+4).w
	move.w	#$100,(g_Player2+4).w
loc_04764C:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorld).l
	lea.l	(Tbl_0097AA).l,a5
	jsr	(Scroll_NextCmd).l
	bsr.w	Stage4_DrawCones
	bsr.w	Stage4_HallFloorRipple
	jsr	(Frame_End).l
	bra.s	loc_04764C
