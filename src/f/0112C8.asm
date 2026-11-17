; ---------------------------------------------------------------------------
; HUD_Update - per-frame HUD work: player aggregates, the two panels' scripts and slides, then the one DMA step selected by g_HUDStep
; In:  none
; Out: g_HUDStep advanced by the step taken
; Evidence: called by the frame start $9B86 on 129k TAS frames; calls Player_GatherStats, $11A1C, HUD_RunPanelScript + $11834 + $1191C per record, then jmp HUD_StepIdle(pc,d0) on g_HUDStep (cleared first)
; ---------------------------------------------------------------------------
HUD_Update:
	bsr.w	Player_GatherStats
	bsr.w	HUD_UpdateGoSign
	lea.l	(g_Player1).l,a6
	bsr.w	HUD_RunPanelScript
	bsr.w	HUD_StepPanelSlide
	lea.l	(Tbl_011974).l,a0
	bsr.w	HUD_DrawPanelSprites
	lea.l	(g_Player2).l,a6
	bsr.w	HUD_RunPanelScript
	bsr.w	HUD_StepPanelSlide
	lea.l	(Tbl_0119AC).l,a0
	bsr.w	HUD_DrawPanelSprites
	move.w	(g_HUDStep).l,d0
	clr.w	(g_HUDStep).l
	jmp	HUD_StepIdle(pc,d0.w)
