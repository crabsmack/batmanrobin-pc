; ---------------------------------------------------------------------------
; Rand_Below - returns an unsigned random number in [0, d0)
; In:  d0.w = range
; Out: d0.l = high word of (unsigned random word * range), zero-extended
; Evidence: Rand_Next then mulu.w with the saved d0, swap, andi.l #$FFFF; 23 callers (a2/a4 ops $ACC, $53D4, $5450 ..., Shake $95EC); 6076 TAS frames
; ---------------------------------------------------------------------------
Rand_Below:
	move.w	d0,-(a7)
	bsr.s	Rand_Next
	mulu.w	(a7)+,d0
	swap	d0
	andi.l	#$FFFF,d0
	rts
