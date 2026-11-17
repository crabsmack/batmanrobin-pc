; ---------------------------------------------------------------------------
; Op4_JitterAccelX - a4 op: adds a random [-w/2, w/2) (8.8) to the object's x acceleration $4E
; In:  a4 = script: word w (8.8)
; Out: NEXT
; Evidence: Rand_Next asr.w #4 * w, asr.l #4, added to $4E(a6); script uses $35A5A, $3B72E, $46968; 5 corpus runs
; ---------------------------------------------------------------------------
Op4_JitterAccelX:
	bsr.w	Rand_Next
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$4E(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
