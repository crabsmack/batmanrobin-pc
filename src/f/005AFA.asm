; ---------------------------------------------------------------------------
; Op4_RandVelRadius - a4 op: adds a random [0, w) (8.8) to the object's radius velocity $36
; In:  a4 = script: word w (8.8)
; Out: NEXT
; Evidence: Rand_Next lsr.w #4 * w, asr.l #4, added to $36(a6); script use $26AB4; 7 corpus runs
; ---------------------------------------------------------------------------
Op4_RandVelRadius:
	bsr.w	Rand_Next
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$36(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_RangeVelRadius - a4 op: adds base + a random [0, w) (8.8) to the object's radius velocity $36
; In:  a4 = script: word base (8.8), word w (8.8)
; Out: NEXT
; Evidence: base<<8 + Rand_Next lsr.w #4 * w >> 4 added to $36(a6); script use $36026 (after Op4_JitterHeading); 17 corpus runs
; ---------------------------------------------------------------------------
Op4_RangeVelRadius:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d1,d0
	add.l	d0,$36(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
