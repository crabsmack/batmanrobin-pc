; ---------------------------------------------------------------------------
; Boss_CatHead_DriverFallY - native fragment of the raster driver's fall loop: g_SceneVars+6 = -(its y), how far the face plate still stands for Boss_CatHead_FallRaster
; In:  a6 = the driver object
; Out: g_SceneVars+6; re-enters the script
; Evidence: after Op2_StepVelocity/ApplyMove at $03BAF6 in the coroutine Scr2_03BAF2 (Scr2_03BB06 sets y -$AA, then cruise y $5D5); TAS f90041-f90392 (319 frames; shots f90150/f90300: the face fallen flat, a dark mechanism behind)
; ---------------------------------------------------------------------------
Boss_CatHead_DriverFallY:
	move.w	$42(a6),d0
	neg.w	d0
	move.w	d0,(g_SceneVars+$6).w
	jsr	($7FE).w
