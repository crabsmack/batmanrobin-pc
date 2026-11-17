; ---------------------------------------------------------------------------
; Boss_CatHead_DriverSway - native fragment of the raster driver's sway loop: g_SceneVars+$C (long) = its y >> 4 (the bulge), g_SceneVars+$10 (word) = its x >> 6 (the shear) for Boss_CatHead_SwayRaster
; In:  a6 = the driver object (x on an oscillator: Op2_AddOsc $E before it)
; Out: g_SceneVars+$C, +$10; re-enters the script
; Evidence: after Op2_StepVelocity/AddOsc/ApplyMove at $03BA5A; TAS f87705-f89520 (1816 frames); RAM f88320: +$C $00000880, +$10 $E178
; ---------------------------------------------------------------------------
Boss_CatHead_DriverSway:
	move.l	$42(a6),d0
	asr.l	#$4,d0
	move.l	d0,(g_SceneVars+$C).w
	move.l	$3E(a6),d0
	asr.l	#$6,d0
	move.w	d0,(g_SceneVars+$10).w
	jsr	($7FE).w
