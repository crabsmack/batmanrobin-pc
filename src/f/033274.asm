; ---------------------------------------------------------------------------
; Flight_RunScene - the flying section's scene: Scene_InitStage (Tbl_032FE2), tile-queue V-int object, palette lines 0/3, the object $2F5F4, runs the pushed loader script, locks the camera, refills both players' health, then loops: objects, camera, g_MapScrollX += 1.5 px, the frame callback g_SceneVars+$98, Frame_End
; In:  (sp) = the a6 loader script pushed by the Stage2_RunFlight* entries
; Out: never returns (the scene ends through Scene_FadeOutExit popping the loop)
; Evidence: reached by bra from the four section entries $3324A-$33270; TAS f35416 init, then the loop until f79303; the callback is set by Flight_StartCorridor/Clouds/SkyOnly/Airship/BeamShip; g_Player1/2 +4 = health ($100 full)
; ---------------------------------------------------------------------------
Flight_RunScene:
	lea.l	Tbl_032FE2(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Layer_FlushTileQueueIfIdle).l,a0
	jsr	(VIntObj_New).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Scr6_032DCE(pc),a0
	jsr	(Pal_LoadLine3).l
	jsr	Flight_SpawnAirshipPathObj(pc)
	movea.l	(a7)+,a6
	jsr	(VM6_Run).l
	jsr	(Camera_LockHere).l
	move.w	#$100,(g_Player1+$4).w
	move.w	#$100,(g_Player2+$4).w
loc_0332BE:
	jsr	(Frame_BeginStage).l
	tst.b	(g_FreeCam).w
	beq.s	loc_0332D2
	jsr	(Debug_FreeCamStep).l
	bra.s	loc_0332E4
loc_0332D2:
	jsr	(Obj_RunList).l
	jsr	(Camera_TrackPlayersY).l
	jsr	(Camera_TrackPlayersX).l
loc_0332E4:
	addi.l	#$180,(g_MapScrollX).w
	movea.l	(g_SceneVars+$98).w,a0
	jsr	(a0)
	jsr	(Frame_End).l
	bra.s	loc_0332BE
; ---------------------------------------------------------------------------
; Flight_StartCorridor - loader-script native of the flying section's start: loads the corridor's x-trigger table Tbl_021D26 (one wave), scroll mode $34, the plane B V-scroll H-int, and frame callback Flight_FrameCorridor
; In:  -
; Out: d0 = 0 (Op6_CallNativeRetry continues)
; Evidence: `70BE 0003 32FA` at $3304A in the $2200 loader Tbl_03301E; TAS f35416-f35433, then Flight_FrameCorridor runs
; ---------------------------------------------------------------------------
Flight_StartCorridor:
	lea.l	(Tbl_021D26).l,a0
	jsr	(Trigger_LoadX).l
	move.w	#$34,d0
	jsr	(Scroll_SetMode).l
	jsr	(HInt_InstallVScrollB).l
	move.l	#Flight_FrameCorridor,(g_SceneVars+$98).w
	moveq	#$0,d0
	rts
