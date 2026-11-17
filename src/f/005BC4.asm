; ---------------------------------------------------------------------------
; Op4_RangeAccelX - a4 op: adds base + a random [0, w) (8.8) to the object's x acceleration $4E
; In:  a4 = script: word base (8.8), word w (8.8)
; Out: NEXT
; Evidence: base<<8 + Rand_Next lsr.w #4 * w >> 4 added to $4E(a6); script use $13226 (Drop child B, `FFF8 FFF9`: a drag); 33 corpus runs
; ---------------------------------------------------------------------------
Op4_RangeAccelX:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d1,d0
	add.l	d0,$4E(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_JitterAccelY - a4 op: adds a random [-w/2, w/2) (8.8) to the object's y acceleration $52
; In:  a4 = script: word w (8.8)
; Out: NEXT
; Evidence: Rand_Next asr.w #4 * w, asr.l #4, added to $52(a6); 13 corpus runs
; ---------------------------------------------------------------------------
Op4_JitterAccelY:
	bsr.w	Rand_Next
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$52(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
