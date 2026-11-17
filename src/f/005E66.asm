; ---------------------------------------------------------------------------
; Op4_ChildScatterVelRadius - a4 op: kicks the last spawned child's radius velocity $36(a5) by base + a random [0, w/2) (8.8), outward or inward at random
; In:  a4 = script: word base (8.8), word w (8.8); a5 = child
; Out: NEXT
; Evidence: Rand_Next asr.w #4 * w, asr.l #4; base << 8 negated when the random part is negative (the Op4_Scatter* code) on $36(a5); script use $26AD8 (`5E66 0100 0100`); 3 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildScatterVelRadius:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	bpl.s	loc_005E7A
	neg.l	d1
loc_005E7A:
	add.l	d1,d0
	add.l	d0,$36(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
