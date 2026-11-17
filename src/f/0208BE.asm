; ---------------------------------------------------------------------------
; Stage1_SpawnSignDebris - spawns the debris burst of a blasted bank sign: one 7-block run-list object (Stage1_SignDebrisRun, 70 frames) holding 18 pieces initialized from Tbl_02095E
; In:  d4/d5 = burst centre, d6 = sprite attr
; Out: a6 = the debris object (or nothing when the pool is short)
; Evidence: Obj_NewMulti with d7 = 7 and handler Stage1_SignDebrisRun; $1A(a6) = $46; 6 sprite blocks x 3 pieces of $18 bytes; called only by Stage1_BlastSignAt (d5 $96, d6 $2000)
; ---------------------------------------------------------------------------
Stage1_SpawnSignDebris:
	moveq	#$7,d7
	lea.l	(Stage1_SignDebrisRun).l,a0
	jsr	(Obj_NewMulti).l
	bne.s	loc_0208F6
	clr.w	$18(a6)
	move.w	#$46,$1A(a6)
	lea.l	(Tbl_02095E).l,a3
	lea.l	$8(a6),a4
	moveq	#$5,d7
loc_0208E4:
	movea.w	(a4)+,a5
	bsr.w	Stage1_InitSignDebris
	bsr.w	Stage1_InitSignDebris
	bsr.w	Stage1_InitSignDebris
	dbra	d7,loc_0208E4
loc_0208F6:
	rts
