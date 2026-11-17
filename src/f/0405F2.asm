; ---------------------------------------------------------------------------
; Stage3_InitHatterRoad - sets up the Hatter road scene: scroll mode $28 (plane A line DMA + H-int raster), installs the plane-A V-scroll H-int, then spawns the road driver, the swirl at the road's far end and the Hatter boss
; In:  -
; Out: -
; Evidence: bsr from Stage3_RunHatterRoad only; calls Stage3_SpawnRoadDriver, Stage3_SpawnRoadEndSwirl, Boss_Hatter_Spawn; TAS f113311 (one frame)
; ---------------------------------------------------------------------------
Stage3_InitHatterRoad:
	move.w	#$28,(g_ScrollMode).l
	jsr	(HInt_InstallVScrollA).l
	bsr.w	Stage3_SpawnRoadDriver
	jsr	Stage3_SpawnRoadEndSwirl(pc)
	jsr	Boss_Hatter_Spawn(pc)
	rts
