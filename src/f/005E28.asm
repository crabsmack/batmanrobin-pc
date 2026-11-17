; ---------------------------------------------------------------------------
; Op4_ChildRandVelY - a4 op: adds a random [0, w) (8.8 px/frame; w may be negative) to the child's y velocity $42(a5)
; In:  a4 = script: word w (8.8); a5 = child
; Out: NEXT
; Evidence: Rand_Next lsr.w #4 * w, asr.l #4, added to $42(a5); 26 a4-script uses, e.g. $1A63C (`5E28 FA00`), $1A68A (`FB00`); 39 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildRandVelY:
	bsr.w	Rand_Next
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$42(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildRangeVelY - a4 op: adds base + a random [0, w) (8.8 px/frame) to the child's y velocity $42(a5)
; In:  a4 = script: word base (8.8), word w (8.8); a5 = child
; Out: NEXT
; Evidence: the Op4_RangeVelY code on $42(a5); script uses $10D9E / $10DB0 (`FFCD FFCD`, skid dust: a small upward hop), $1A3AA; 95 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildRangeVelY:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d1,d0
	add.l	d0,$42(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
