; ---------------------------------------------------------------------------
; Flight_FrameCorridor - frame callback of the corridor part: runs the x-triggers and Flight_UpdateCorridor (the perspective corridor on plane B)
; In:  -
; Out: -
; Evidence: stored in g_SceneVars+$98 by Flight_StartCorridor(NoWaves), called by Flight_RunScene every frame; TAS f35433-f46290 (png f040200: the corridor)
; ---------------------------------------------------------------------------
Flight_FrameCorridor:
	jsr	(Trigger_RunX).l
	jmp	(Flight_UpdateCorridor).l
; ---------------------------------------------------------------------------
; Flight_StartClouds - loader-script native at the corridor's end: loads the x-trigger table Tbl_021D3A (weapon drop + wave), scroll mode $38, frame callback Flight_FrameCorridorToClouds, builds the cloud-bank wobble table and sets g_SceneVars+$A3 (cloud bank on screen)
; In:  -
; Out: d0 = 0
; Evidence: `70BE 0003 3356` at $33082 (second segment of Tbl_03301E, started from the wave script at $021D90) and at $3317A in Scr6_033120; TAS f46290 (png f046800: the corridor's exit, f047400 the clouds)
; ---------------------------------------------------------------------------
Flight_StartClouds:
	lea.l	(Scr5_021D3A).l,a0
	jsr	(Trigger_LoadX).l
	move.w	#$38,(g_ScrollMode).w
	move.l	#Flight_FrameCorridorToClouds,(g_SceneVars+$98).w
	bsr.w	Flight_BuildWobbleTable
	st.b	(g_SceneVars+$A3).w
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Flight_WaitCloudsPassed - Op6_CallNativeRetry wait: returns g_SceneVars+$A3, so the loader script waits while the cloud bank is still on screen (Flight_FrameClouds clears it at layer x $E40)
; In:  -
; Out: d0 = g_SceneVars+$A3 (nonzero = retry next frame)
; Evidence: `70BE 0003 337C` at $33092, right after `70E0 772E 2400` (section id $2400) in Tbl_03301E; TAS f46290-f49938
; ---------------------------------------------------------------------------
Flight_WaitCloudsPassed:
	move.b	(g_SceneVars+$A3).w,d0
	rts
