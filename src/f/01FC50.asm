; ---------------------------------------------------------------------------
; Stage1_RunGemExpo - section $1300: the stage 1 Gem Expo interior: music 8, a6 scene script Scr6_01FBCA, palettes, plane A drawn, trigger table Tbl_0163BA, both players' health refilled, then a frame loop with the layer-A scroll and the parallax list Tbl_00970A
; In:  -
; Out: never returns normally
; Evidence: Tbl_00A0AC maps $1300 here; Layer_DrawScreen, Layer_FlushTileQueueIfIdle as V-int object, g_Player1/2+4 = $100; Frame_RunWorldLayerA, Layer_ScrollStep, Scroll_RunParallax; TAS f6334; shot f7200 (windowed hall, red carpet, GEM EXPO sign at f9360)
; ---------------------------------------------------------------------------
Stage1_RunGemExpo:
	move.w	#$8,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Scr6_01FBCA(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_01FA36(pc),a0
	jsr	(Pal_LoadLine2).l
	jsr	(Pal_LoadLine3).l
	lea.l	Scr6_01FC06(pc),a6
	jsr	(VM6_Run).l
	lea.l	(g_LayerA).l,a6
	jsr	(Layer_DrawScreen).l
	lea.l	(Layer_FlushTileQueueIfIdle).l,a0
	jsr	(VIntObj_New).l
	lea.l	(Tbl_0163BA).l,a0
	jsr	(Trigger_LoadX).l
	move.w	#$100,(g_Player1+$4).w
	move.w	#$100,(g_Player2+$4).w
loc_01FCBA:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorldLayerA).l
	lea.l	(g_LayerA).l,a6
	jsr	(Layer_ScrollStep).l
	lea.l	(Tbl_00970A).l,a5
	jsr	(Scroll_RunParallax).l
	jsr	(Frame_End).l
	bra.s	loc_01FCBA
