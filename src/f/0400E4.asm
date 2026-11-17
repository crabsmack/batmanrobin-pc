; ---------------------------------------------------------------------------
; Stage3_RunMushroomForest - section $3200 routine, the stage 3 mushroom forest: music $E, scene Scr6_040018, palettes, a6 loader Scr6_040054, draws plane A, V-int tile flush, triggers Tbl_033F30, full health for both players; frame loop with Layer_ScrollStep and parallax bands Tbl_00976C
; In:  -
; Out: never returns (Scene_FadeOutExit)
; Evidence: Tbl_00A0AC entry $3200 -> $0400E4; TAS f90392 (one frame), level-skip f8835; png f090600: giant mushrooms and trees
; ---------------------------------------------------------------------------
Stage3_RunMushroomForest:
	move.w	#$E,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Scr6_040018(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_03FDE8(pc),a0
	jsr	(Pal_LoadLine3).l
	lea.l	Scr6_040054(pc),a6
	jsr	(VM6_Run).l
	lea.l	(g_LayerA).l,a6
	jsr	(Layer_DrawScreen).l
	lea.l	(Layer_FlushTileQueueIfIdle).l,a0
	jsr	(VIntObj_New).l
	lea.l	(Tbl_033F30).l,a0
	jsr	(Trigger_LoadX).l
	jsr	(Camera_UpdateView).l
	move.w	#$100,(g_Player1+$4).w
	move.w	#$100,(g_Player2+$4).w
loc_04014E:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorldLayerA).l
	lea.l	(g_LayerA).l,a6
	jsr	(Layer_ScrollStep).l
	lea.l	(Tbl_00976C).l,a5
	jsr	(Scroll_RunParallax).l
	jsr	(Frame_End).l
	bra.s	loc_04014E
