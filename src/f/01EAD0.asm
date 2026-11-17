; ---------------------------------------------------------------------------
; Boss_Claw_PlaceFingers - spins the claw's finger frame (g_Rot3DInner angle +0 += 8 a frame) and places the three fingers 120 degrees apart ($156, $156, $154) into their point lists
; In:  a6 = the claw (+$10/+$12/+$14 = finger blocks)
; Out: the finger blocks filled by Boss_Claw_PlaceFinger
; Evidence: callers Boss_Claw_SwingSideways/SwingAtCrane/SwingFingers; 1301 TAS frames f10512-f12924
; ---------------------------------------------------------------------------
Boss_Claw_PlaceFingers:
	movem.l	a2-a6,-(a7)
	lea.l	(g_Rot3DInner).l,a0
	addq.w	#$8,(a0)
	movea.w	$10(a6),a5
	addi.w	#$156,(a0)
	bsr.w	Boss_Claw_PlaceFinger
	movea.w	$12(a6),a5
	addi.w	#$156,(a0)
	bsr.w	Boss_Claw_PlaceFinger
	movea.w	$14(a6),a5
	addi.w	#$154,(a0)
	bsr.w	Boss_Claw_PlaceFinger
	movem.l	(a7)+,a2-a6
	rts
