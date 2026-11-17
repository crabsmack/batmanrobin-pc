; ---------------------------------------------------------------------------
; Flight_FrameCorridorToClouds - frame callback at the corridor's exit: x-triggers; while g_LayerA x < $680 the corridor (Flight_UpdateCorridor) and the cloud bank both draw; then scroll mode $C, loader Tbl_0330FC (sky art) and the callback becomes Flight_FrameClouds
; In:  -
; Out: -
; Evidence: stored by Flight_StartClouds; TAS f46291-f47955 (the cloud bank scrolls 1 px a frame: $680 px = 1664 frames); png f046800/f047400
; ---------------------------------------------------------------------------
Flight_FrameCorridorToClouds:
	jsr	(Trigger_RunX).l
	cmpi.w	#$680,(g_LayerA).w
	blt.w	Flight_FrameCorridorClouds
	move.w	#$C,d0
	jsr	(Scroll_SetMode).l
	lea.l	Tbl_0330FC(pc),a6
	jsr	(VM6_Start).l
	move.l	#Flight_FrameClouds,(g_SceneVars+$98).w
; ---------------------------------------------------------------------------
; Flight_FrameClouds - frame callback of the cloud bank: until g_LayerA x reaches $E40 it scrolls the wobbling cloud bank (Flight_ScrollCloudBank) over the sky; then clears g_SceneVars+$A3 (releasing Flight_WaitCloudsPassed) and leaves only Flight_DrawSky
; In:  -
; Out: -
; Evidence: stored by Flight_FrameCorridorToClouds (`move.l #$3342E`); TAS f47955-f49938 ($7C0 px at 1 px a frame); png f049200: cloud bank over the dark sky
; ---------------------------------------------------------------------------
Flight_FrameClouds:
	cmpi.w	#$E40,(g_LayerA).w
	blt.s	loc_033444
	sf.b	(g_SceneVars+$A3).w
	move.l	#Flight_DrawSky,(g_SceneVars+$98).w
	rts
loc_033444:
	bsr.w	Flight_ScrollCloudBank
