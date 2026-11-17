; ---------------------------------------------------------------------------
; Attract_Skyscraper - third attract scene (Tbl_00A152): the camera climbs a skyscraper under a red storm sky; plain scene init + art, plane A layer placed $6E8 px down and drawn, a V-int tile-flush object, g_IntroNoSkip cleared, the camera and plane-B objects, skippable loop
; In:  none
; Out: via Scene_LoopSkippable
; Evidence: Tbl_00A152 entry 3; Layer_MoveYWithCamera $6E8, Layer_DrawScreen, VIntObj_New(Layer_FlushTileQueueIfIdle), sf g_IntroNoSkip; TAS f364, idle 364 + 3925n (png f600 window close-up, f4800 tower top)
; ---------------------------------------------------------------------------
Attract_Skyscraper:
	lea.l	Scr6_0489E2(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_048A06(pc),a6
	jsr	(VM6_Run).l
	lea.l	(g_LayerA).l,a6
	move.w	#$6E8,d0
	jsr	(Layer_MoveYWithCamera).l
	jsr	(Layer_DrawScreen).l
	lea.l	(Layer_FlushTileQueueIfIdle).l,a0
	jsr	(VIntObj_New).l
	sf.b	(g_IntroNoSkip).w
	bsr.w	Attract_SpawnSkyscraperCamera
	bsr.w	Attract_SpawnPlaneBDriver
	jsr	(Camera_UpdateView).l
	bra.w	Scene_LoopSkippable
