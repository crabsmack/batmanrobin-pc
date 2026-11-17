; ---------------------------------------------------------------------------
; Flight_AirshipSetMapOrigin - native op of the airship path object: publishes its x/y as g_SceneVars+$94/$96 (map position of world 0,0: the airship map scrolls with it), then falls into Flight_AirshipRunTriggers
; In:  a6 = the path object (Scr2_02F644)
; Out: falls through to Flight_AirshipRunTriggers
; Evidence: op $2CB2 at $2F674 in the path loop (step velocity, oscillator, ApplyMove, this, yield); the frame callback Flight_FrameAirship ($333B2) sets plane A to g_SceneVars+$94/$96 + g_ViewX/Y; TAS 67264-77147 every frame
; ---------------------------------------------------------------------------
Flight_AirshipSetMapOrigin:
	move.w	$3E(a6),(g_SceneVars+$94).w
	move.w	$42(a6),(g_SceneVars+$96).w
