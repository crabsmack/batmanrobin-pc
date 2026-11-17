; ---------------------------------------------------------------------------
; Rand_Signed - returns a signed random number in [-d0/2, d0/2)
; In:  d0.w = range
; Out: d0.l = high word of (signed random word * range), sign-extended
; Evidence: Rand_Next then muls.w with the saved d0 and swap; 5 callers ($1736, $3E58, $10D12, $13F5E, $2417E); 1766 TAS frames
; ---------------------------------------------------------------------------
Rand_Signed:
	move.w	d0,-(a7)
	bsr.s	Rand_Next
	muls.w	(a7)+,d0
	swap	d0
	ext.l	d0
	rts
