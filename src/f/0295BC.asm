; ---------------------------------------------------------------------------
; Boss_BeamShip_RadiusFromX - native fragment in the radius object's loop: g_BeamRadius = its x, i.e. the eased radius of the beam disc
; In:  a6 = the radius object
; Out: g_BeamRadius
; Evidence: Op2_Native at $295BA in Scr2_029598; $33A88 draws the disc with this radius; TAS 77185-79013
; ---------------------------------------------------------------------------
Boss_BeamShip_RadiusFromX:
	move.w	$3E(a6),(g_BeamRadius).w
	jsr	($7FE).w
