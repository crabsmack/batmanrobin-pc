; ---------------------------------------------------------------------------
; Boss_CatHead_DriverCurtainY - native fragment of the raster driver's curtain loop: stores its y ($42(a6)) in g_SceneVars+4, the curtain height Boss_CatHead_CurtainRaster draws
; In:  a6 = the driver object
; Out: g_SceneVars+4; re-enters the script
; Evidence: after Op2_StepVelocity/ApplyMove at $03B96C in the coroutine started with Boss_CatHead_StartCurtainRaster; TAS f87285-f90022 (908 frames: the curtain rising at the start, closing and reopening after the defeat, shots f87400/f87560/f89640/f89880)
; ---------------------------------------------------------------------------
Boss_CatHead_DriverCurtainY:
	move.w	$42(a6),(g_SceneVars+$4).w
	jsr	($7FE).w
