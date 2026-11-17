; ---------------------------------------------------------------------------
; Op4_ScatterVelY - a4 op: kicks the object's y velocity $42 by base + a random [0, w/2) (8.8) up or down at random
; In:  a4 = script: word base (8.8), word w (8.8)
; Out: NEXT
; Evidence: the Op4_ScatterVelX code on $42(a6); script use $1322C (Drop child B); 29 corpus runs
; ---------------------------------------------------------------------------
Op4_ScatterVelY:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	bpl.s	loc_005A94
	neg.l	d1
loc_005A94:
	add.l	d1,d0
	add.l	d0,$42(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_RandVelY - a4 op: adds a random [0, w) (8.8 px/frame; w may be negative) to the object's y velocity $42
; In:  a4 = script: word w (8.8)
; Out: NEXT
; Evidence: Rand_Next lsr.w #4 * w, asr.l #4, added to $42(a6); script use $1F1F6 (`5A9E FE00`: up to -2 px/frame); 3 corpus runs
; ---------------------------------------------------------------------------
Op4_RandVelY:
	bsr.w	Rand_Next
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$42(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_RangeVelY - a4 op: adds base + a random [0, w) (8.8 px/frame) to the object's y velocity $42
; In:  a4 = script: word base (8.8), word w (8.8)
; Out: NEXT
; Evidence: the Op4_RangeVelX code on $42(a6); script uses $13256 (Drop child A, `0100 0100`), $2438A (`FA00 FC00`: a toss upward); 41 corpus runs
; ---------------------------------------------------------------------------
Op4_RangeVelY:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d1,d0
	add.l	d0,$42(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_JitterVelRadius - a4 op: adds a random [-w/2, w/2) (8.8) to the object's radius velocity $36
; In:  a4 = script: word w (8.8)
; Out: NEXT
; Evidence: Rand_Next asr.w #4 * w, asr.l #4, added to $36(a6); script uses $26BB4, $26BD2; 6 corpus runs
; ---------------------------------------------------------------------------
Op4_JitterVelRadius:
	bsr.w	Rand_Next
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$36(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
