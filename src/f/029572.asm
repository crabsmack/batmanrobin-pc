; ---------------------------------------------------------------------------
; Boss_BeamShip_RunRadiusObj - runs the ship's unlinked radius object inline with its target x (a3 +$14) = the ship's var 0, so the object's x eases toward the wanted beam radius
; In:  a6/a4 = the ship (falls in from Boss_BeamShip_SetShipVertex)
; Out: a1-a6 and g_VM2SavedSP preserved
; Evidence: the radius object's script Scr2_029598 seeks its screen x to the target (Op2_SeekScreenX) and Boss_BeamShip_RadiusFromX copies that x to g_BeamRadius; TAS 77185-79028 (1503 frames)
; ---------------------------------------------------------------------------
Boss_BeamShip_RunRadiusObj:
	movem.l	a1-a6,-(a7)
	move.l	(g_VM2SavedSP).w,-(a7)
	movea.l	$22(a6),a6
	movea.w	$8(a6),a0
	move.w	$22(a4),$14(a0)
	jsr	(VM2_RunObject).l
	move.l	(a7)+,(g_VM2SavedSP).w
	movem.l	(a7)+,a1-a6
	rts
