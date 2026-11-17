; ---------------------------------------------------------------------------
; Boss_Claw_PlaceFinger - builds the finger frame's matrix and transforms the 3 points of a finger (Tbl_ClawFingerPoints) into a5 as 10-byte entries (x, y + $10E, z, size word, attr word) for Boss_Claw_DrawPoints
; In:  a0 = g_Rot3DInner, a5 = the finger's point list
; Out: a5 past 3 entries
; Evidence: Rot3D_BuildMatrix + Rot3D_TransformPoint per point, the long after each point copied; called 3 times per Boss_Claw_PlaceFingers
; ---------------------------------------------------------------------------
Boss_Claw_PlaceFinger:
	jsr	(Rot3D_BuildMatrix).l
	lea.l	(Tbl_ClawFingerPoints).l,a1
	moveq	#$2,d6
loc_01EB14:
	jsr	(Rot3D_TransformPoint).l
	addi.w	#$10E,d5
	move.w	d4,(a5)+
	move.w	d5,(a5)+
	move.w	d7,(a5)+
	move.l	(a1)+,(a5)+
	dbra	d6,loc_01EB14
	rts
