; ---------------------------------------------------------------------------
; Stage3_RunBacklot - section $3100 routine, stage 3's first scene (the film-set backlot): music $E, stage scene Tbl_03FE08, palettes, a6 loader Tbl_03FE44, draws plane A, the V-int tile-queue flush object, triggers Tbl_033CBE, full health for both players; then the endless frame loop (world + layer A, Layer_ScrollStep, parallax bands Tbl_00974E)
; In:  -
; Out: never returns: the scene ends through Scene_FadeOutExit popping the loop's return
; Evidence: Tbl_00A0AC entry $3100 -> $03FE94; TAS f79329 (one frame), level-skip f7764; png f079800: the backlot with spotlights, scaffolds and a painted night backdrop
; ---------------------------------------------------------------------------
Stage3_RunBacklot:
	move.w	#$E,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Tbl_03FE08(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_03FDE8(pc),a0
	jsr	(Pal_LoadLine3).l
	lea.l	Tbl_03FE44(pc),a6
	jsr	(VM6_Run).l
	lea.l	(g_LayerA).l,a6
	jsr	(Layer_DrawScreen).l
	lea.l	(Layer_FlushTileQueueIfIdle).l,a0
	jsr	(VIntObj_New).l
	lea.l	(Tbl_033CBE).l,a0
	jsr	(Trigger_LoadX).l
	jsr	(Camera_UpdateView).l
	move.w	#$100,(g_Player1+$4).w
	move.w	#$100,(g_Player2+$4).w
loc_03FEFE:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorldLayerA).l
	lea.l	(g_LayerA).l,a6
	jsr	(Layer_ScrollStep).l
	lea.l	(Tbl_00974E).l,a5
	jsr	(Scroll_RunParallax).l
	jsr	(Frame_End).l
	bra.s	loc_03FEFE
