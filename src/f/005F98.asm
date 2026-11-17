; ---------------------------------------------------------------------------
; Op4_ChildRandAccelY - a4 op: adds a random [0, w) (8.8; w may be negative) to the child's y acceleration $52(a5)
; In:  a4 = script: word w (8.8); a5 = child
; Out: NEXT
; Evidence: Rand_Next lsr.w #4 * w, asr.l #4, added to $52(a5); script use $4955A (`5F98 FFF4`); 1 corpus run
; ---------------------------------------------------------------------------
Op4_ChildRandAccelY:
	bsr.w	Rand_Next
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$52(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
