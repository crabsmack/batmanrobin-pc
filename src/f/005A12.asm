; ---------------------------------------------------------------------------
; Op4_JitterVelX - a4 op: adds a random [-w/2, w/2) (8.8 px/frame) to the object's x velocity $3E
; In:  a4 = script: word w (8.8)
; Out: NEXT
; Evidence: Rand_Next asr.w #4 (signed 12 bits) * w, asr.l #4, added to $3E(a6); 6 corpus runs
; ---------------------------------------------------------------------------
Op4_JitterVelX:
	bsr.w	Rand_Next
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$3E(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ScatterVelX - a4 op: kicks the object's x velocity $3E by base + a random [0, w/2) (8.8) in a random direction (left or right)
; In:  a4 = script: word base (8.8), word w (8.8)
; Out: NEXT
; Evidence: the signed random part r = +-w/2 is added to base<<8 negated when r < 0, so |kick| is in [base, base + w/2); script uses $13262 (`5A24 0033 0080` in Drop child A), $1F1FA, $24396; 33 corpus runs
; ---------------------------------------------------------------------------
Op4_ScatterVelX:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	bpl.s	loc_005A38
	neg.l	d1
loc_005A38:
	add.l	d1,d0
	add.l	d0,$3E(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
