; ---------------------------------------------------------------------------
; Stage1_RunWarehouse - section $1301: the stage 1 warehouse (crates, the crane-claw boss): music 1, a6 scene script Scr6_01FCE6, trigger table Tbl_016632, then a frame loop with the layer-A scroll and the parallax list Tbl_00972C
; In:  -
; Out: never returns normally
; Evidence: Tbl_00A0AC maps $1301 here; no health refill, Camera_UpdateView before the loop; the loop loads a5 (not a6) with g_LayerA before Layer_ScrollStep; TAS f9615; shots f10800-f12660 (crates, Harley's claw)
; ---------------------------------------------------------------------------
Stage1_RunWarehouse:
	move.w	#$1,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Scr6_01FCE6(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Scr6_01FD22(pc),a6
	jsr	(VM6_Run).l
	lea.l	(g_LayerA).l,a6
	jsr	(Layer_DrawScreen).l
	lea.l	(Layer_FlushTileQueueIfIdle).l,a0
	jsr	(VIntObj_New).l
	lea.l	(Tbl_016632).l,a0
	jsr	(Trigger_LoadX).l
	jsr	(Camera_UpdateView).l
loc_01FDCA:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorldLayerA).l
	lea.l	(g_LayerA).l,a5
	jsr	(Layer_ScrollStep).l
	lea.l	(Tbl_00972C).l,a5
	jsr	(Scroll_RunParallax).l
	jsr	(Frame_End).l
	bra.s	loc_01FDCA
