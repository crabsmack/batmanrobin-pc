; ---------------------------------------------------------------------------
; Op4_ChildJitterAccelX - a4 op: adds a random [-w/2, w/2) (8.8) to the last spawned child's x acceleration $4E(a5)
; In:  a4 = script: word w (8.8); a5 = child
; Out: NEXT
; Evidence: Rand_Next asr.w #4 * w, asr.l #4, added to $4E(a5); script uses $3582A (`5F0C 0013`), $3CE58; 8 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildJitterAccelX:
	bsr.w	Rand_Next
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$4E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildScatterAccelX - a4 op: kicks the child's x acceleration $4E(a5) by base + a random [0, w/2) (8.8), left or right at random
; In:  a4 = script: word base (8.8), word w (8.8); a5 = child
; Out: NEXT
; Evidence: the Op4_Scatter* code on $4E(a5); script uses $36F98 (`5F1E 0005 0019`), $37D62; 30 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildScatterAccelX:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	bpl.s	loc_005F32
	neg.l	d1
loc_005F32:
	add.l	d1,d0
	add.l	d0,$4E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
