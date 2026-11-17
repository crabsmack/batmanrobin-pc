; ---------------------------------------------------------------------------
; Op4_ChildRangeAccelX - a4 op: adds base + a random [0, w) (8.8) to the child's x acceleration $4E(a5)
; In:  a4 = script: word base (8.8), word w (8.8); a5 = child
; Out: NEXT
; Evidence: Rand_Next lsr.w #4 * w, asr.l #4, plus base << 8, added to $4E(a5); script use $28F56 (`5F4E FFC8 0013`); 1 corpus run
; ---------------------------------------------------------------------------
Op4_ChildRangeAccelX:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d1,d0
	add.l	d0,$4E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildJitterAccelY - a4 op: adds a random [-w/2, w/2) (8.8) to the child's y acceleration $52(a5)
; In:  a4 = script: word w (8.8); a5 = child
; Out: NEXT
; Evidence: Rand_Next asr.w #4 * w, asr.l #4, added to $52(a5); script uses $28F5C (`5F68 000C`), $28F8A; 1 corpus run
; ---------------------------------------------------------------------------
Op4_ChildJitterAccelY:
	bsr.w	Rand_Next
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$52(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
