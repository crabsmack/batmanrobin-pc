; ---------------------------------------------------------------------------
; Flight_AirshipStartPath - starts the airship section of the flight: loads the trigger table Tbl_0307C2 (130 entries: lights, turrets, domes, pipe cannons, the dome-cannon boss), points the parked path object's script at Scr2_02F644 and puts it and g_SceneVars+$94/$96 at (-$154, $2C)
; In:  g_SceneVars+$92
; Out: a4/a6 = the path object
; Evidence: jsr from the loader native Flight_StartAirship ($33398) (which then sets scroll mode $C and the map-frame callback Flight_FrameAirship ($333B2)); Scr2_02F644 = speed $100, music 5, cruise x $AA, Y oscillator, coroutine 0 and a loop calling Flight_AirshipSetMapOrigin; TAS f67262 (the airship hull appears, png f067800)
; ---------------------------------------------------------------------------
Flight_AirshipStartPath:
	lea.l	(Tbl_0307C2).l,a0
	jsr	(Trigger_LoadX).l
	movea.w	(g_SceneVars+$92).w,a6
	movea.w	$A(a6),a4
	move.l	#$2F644,$2A(a4)
	move.w	#$FEAC,d0
	move.w	#$2C,d1
	move.w	d0,(g_SceneVars+$94).w
	move.w	d0,$3E(a6)
	move.w	d1,(g_SceneVars+$96).w
	move.w	d1,$42(a6)
	rts
