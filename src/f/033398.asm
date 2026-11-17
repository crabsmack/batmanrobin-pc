; ---------------------------------------------------------------------------
; Flight_StartAirship - loader-script native of the airship part: the setup $2F60E (trigger table Tbl_0307C2, hull map offset g_SceneVars+$94/$96 = -$154, $2C), scroll mode $C and frame callback Flight_FrameAirship
; In:  -
; Out: d0 = 0
; Evidence: `70BE 0003 3398` at $330C6 (Tbl_03301E) and $331E8 (Scr6_033182); TAS f67262 (png f067800: the hull's nose; f072000 its side)
; ---------------------------------------------------------------------------
Flight_StartAirship:
	jsr	Flight_AirshipStartPath(pc)
	move.w	#$C,d0
	jsr	(Scroll_SetMode).l
	move.l	#Flight_FrameAirship,(g_SceneVars+$98).w
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Flight_FrameAirship - frame callback of the airship part: the sky (Flight_DrawSky), then plane A's hull map follows the camera plus the offset g_SceneVars+$94/$96 (Layer_ScrollStep, Layer_AnimateTiles) and the scroll command list Tbl_0333F2 sets its line scroll
; In:  -
; Out: -
; Evidence: stored in g_SceneVars+$98 by Flight_StartAirship; TAS f67262-f77184 (png f067800-f076800)
; ---------------------------------------------------------------------------
Flight_FrameAirship:
	bsr.w	Flight_DrawSky
	lea.l	(g_LayerA).l,a6
	move.w	(g_SceneVars+$94).w,d0
	add.w	(g_ViewX).w,d0
	sub.w	(a6),d0
	move.w	d0,$4(a6)
	move.w	(g_SceneVars+$96).w,d0
	add.w	(g_ViewY).w,d0
	sub.w	$2(a6),d0
	move.w	d0,$6(a6)
	jsr	(Layer_ScrollStep).l
	jsr	(Layer_AnimateTiles).l
	lea.l	(Tbl_0333F2).l,a5
	jmp	(Scroll_NextCmd).l
