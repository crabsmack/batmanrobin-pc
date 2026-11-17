; ---------------------------------------------------------------------------
; Op4_JitterX - a4 op: moves the object's x $2E by a random [-w/2, w/2) px (16.16)
; In:  a4 = script: word w
; Out: NEXT
; Evidence: Rand_Next (signed word) * w added to the long $2E(a6); script uses $26AA8, $3709E; 42 corpus runs
; ---------------------------------------------------------------------------
Op4_JitterX:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,$2E(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
