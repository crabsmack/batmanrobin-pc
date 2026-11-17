; ---------------------------------------------------------------------------
; Flight_AirshipRunTriggers - trigger runner of the airship section in map space: fires the g_TriggerList entries whose x key lies in [g_CameraX + g_SceneVars+$94, +$17C] and places each spawned object at its map position (drawn $4E/$50 = entry x/y, then Flight_AirshipMapToWorld)
; In:  g_TriggerNext/g_TriggerTable (Tbl_0307C2 via Flight_AirshipStartPath), g_SceneVars+$94/$96
; Out: g_TriggerNext advanced past the fired entries; a1-a6 preserved
; Evidence: fall-through from Flight_AirshipSetMapOrigin; calls Trigger_Fire (d0 != 0 = retry later); TAS 67264-77147
; ---------------------------------------------------------------------------
Flight_AirshipRunTriggers:
	movem.l	a1-a6,-(a7)
	movea.l	(g_TriggerNext).w,a2
	movea.l	(g_TriggerTable).w,a3
	move.w	(g_CameraX).w,d6
	add.w	(g_SceneVars+$94).w,d6
	move.w	d6,d7
	addi.w	#$17C,d7
loc_02F5A6:
	move.l	(a2)+,d0
	swap	d0
	bmi.s	loc_02F5CE
	cmp.w	d7,d0
	bgt.s	loc_02F5CE
	cmp.w	d6,d0
	blt.s	loc_02F5A6
	jsr	(Trigger_Fire).l
	bne.s	loc_02F5CE
	move.w	$3E(a6),$4E(a6)
	move.w	$42(a6),$50(a6)
	bsr.w	Flight_AirshipMapToWorld
	bra.s	loc_02F5A6
loc_02F5CE:
	subq.l	#$4,a2
	move.l	a2,(g_TriggerNext).w
	movem.l	(a7)+,a1-a6
	rts
