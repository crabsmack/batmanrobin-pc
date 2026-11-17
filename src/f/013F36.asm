; ---------------------------------------------------------------------------
; WeaponDrop_Stop - the director's stop command: clears g_WeaponDropper and kills the director object
; In:  a6 = the director (entered through Op2_Native at $13F34)
; Out: g_WeaponDropper = 0; then Op2_Kill
; Evidence: $13F34 is the stop script given by the trigger at $2355C and the natives at $29334/$30144/$3D0FA; 6 TAS frames (46593-114869)
; ---------------------------------------------------------------------------
WeaponDrop_Stop:
	clr.w	(g_WeaponDropper).w
	jsr	($7FE).w
