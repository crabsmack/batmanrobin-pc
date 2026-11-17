; ---------------------------------------------------------------------------
; Boss_BeamShip_ReleaseRadiusObj - native fragment at the end of the ship: links its radius object onto the main object list ($88DA) so the death disc keeps running after the ship is killed
; In:  a6 = the ship
; Out: continues the a2 script (award score, Op2_Kill)
; Evidence: Op2_Native at $29462; the radius object's death script then shrinks the disc and calls Section_StartExit ($2964C); TAS f79029
; ---------------------------------------------------------------------------
Boss_BeamShip_ReleaseRadiusObj:
	movea.l	$22(a6),a0
	jsr	(Obj_LinkAtEnd).l
	jsr	($7FE).w
