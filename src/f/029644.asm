; ---------------------------------------------------------------------------
; Boss_BeamShip_ClearRadius - native fragment at the end of the death disc: g_BeamRadius = 0 (nothing drawn); the script then calls Section_StartExit and kills the object
; In:  a6 = the radius object
; Out: g_BeamRadius = 0
; Evidence: Op2_Native at $29642, followed by Scr2_02964C `2CBC Section_StartExit / Op2_Kill`; TAS f79108 (the flight section ends)
; ---------------------------------------------------------------------------
Boss_BeamShip_ClearRadius:
	clr.w	(g_BeamRadius).w
	jsr	($7FE).w
