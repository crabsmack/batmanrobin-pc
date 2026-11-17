; ---------------------------------------------------------------------------
; Flight_StartCorridorNoWaves - Flight_StartCorridor for the $2300 restart: the same corridor scroll mode $34, H-int and frame callback, but with the empty trigger table Tbl_012ECE (count 0), so no corridor wave starts
; In:  -
; Out: d0 = 0
; Evidence: `70BE 0003 3322` at $3314C in Scr6_033120 (Stage2_RunFlightClouds), followed by the cloud art and Flight_StartClouds; Tbl_012ECE = word 0; level-skip scenario f5161
; ---------------------------------------------------------------------------
Flight_StartCorridorNoWaves:
	lea.l	(Tbl_012ECE).l,a0
	jsr	(Trigger_LoadX).l
	move.w	#$34,d0
	jsr	(Scroll_SetMode).l
	jsr	(HInt_InstallVScrollB).l
	move.l	#Flight_FrameCorridor,(g_SceneVars+$98).w
	moveq	#$0,d0
	rts
