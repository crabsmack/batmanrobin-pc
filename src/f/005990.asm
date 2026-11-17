; ---------------------------------------------------------------------------
; Op4_JitterXY - a4 op: moves the object's x $2E and y $32 by random [-wx/2, wx/2) / [-wy/2, wy/2) px
; In:  a4 = script: word wx, word wy
; Out: falls into Op4_JitterY for wy, then NEXT
; Evidence: two Rand_Next * w adds to $2E/$32(a6); script uses $35A48, $46956, $4697E; 3 corpus runs
; ---------------------------------------------------------------------------
Op4_JitterXY:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,$2E(a6)
; ---------------------------------------------------------------------------
; Op4_JitterY - a4 op: moves the object's y $32 by a random [-w/2, w/2) px
; In:  a4 = script: word w
; Out: NEXT
; Evidence: label-only entry inside Op4_JitterXY (computed-jump target in 29 corpus runs); script use $370A2, right after Op4_JitterX
; ---------------------------------------------------------------------------
Op4_JitterY:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,$32(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
