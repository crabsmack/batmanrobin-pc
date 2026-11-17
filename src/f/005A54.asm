; ---------------------------------------------------------------------------
; Op4_RangeVelX - a4 op: adds base + a random [0, w) (8.8 px/frame) to the object's x velocity $3E
; In:  a4 = script: word base (8.8), word w (8.8)
; Out: NEXT
; Evidence: Rand_Next lsr.w #4 (unsigned 12 bits) * w, asr.l #4, plus base<<8, added to $3E(a6); script uses $13220 (Drop child B), $305F4; 33 corpus runs
; ---------------------------------------------------------------------------
Op4_RangeVelX:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d1,d0
	add.l	d0,$3E(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
