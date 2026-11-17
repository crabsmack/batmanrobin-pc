; ---------------------------------------------------------------------------
; Op4_ChildRangeAccelY - a4 op: adds base + a random [0, w) (8.8) to the child's y acceleration $52(a5) (gravity spread for debris)
; In:  a4 = script: word base (8.8), word w (8.8); a5 = child
; Out: NEXT
; Evidence: the Op4_Range* code on $52(a5); script uses $35824 (`5FAA 0026 0026`), $36F92; 37 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildRangeAccelY:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	lsr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d1,d0
	add.l	d0,$52(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
