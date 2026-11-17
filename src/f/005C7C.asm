; ---------------------------------------------------------------------------
; Op4_RangeAccelRadius - a4 op: adds base + a random [0, w) (8.8) to the object's radius acceleration $46
; In:  a4 = script: word base (8.8), word w (8.8)
; Out: NEXT
; Evidence: base<<8 + Rand_Next lsr.w #4 * w >> 4 added to $46(a6); script use $26AB8 (after Op4_RandVelRadius); 7 corpus runs
; ---------------------------------------------------------------------------
Op4_RangeAccelRadius:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d1,d0
	add.l	d0,$46(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
