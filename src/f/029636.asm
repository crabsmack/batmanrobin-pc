; ---------------------------------------------------------------------------
; Boss_BeamShip_DeathRadiusFromX - native fragment in the death disc's shrink loop: g_BeamRadius = the radius object's x
; In:  a6 = the radius object
; Out: g_BeamRadius
; Evidence: Op2_Native at $29634 after Op2_SeekScreenX4/StepVelocity/ApplyMove in Scr2_0295D4; TAS 79014-79108
; ---------------------------------------------------------------------------
Boss_BeamShip_DeathRadiusFromX:
	move.w	$3E(a6),(g_BeamRadius).w
	jsr	($7FE).w
