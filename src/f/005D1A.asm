; ---------------------------------------------------------------------------
; Op4_ChildJitterXY - a4 op: moves the child's x/y ($2E/$32 of a5) by random [-wx/2, wx/2) / [-wy/2, wy/2) px
; In:  a4 = script: word wx, word wy; a5 = child
; Out: falls into Op4_ChildJitterY for wy, then NEXT
; Evidence: two Rand_Next * w adds to $2E/$32(a5); 26 a4-script uses, e.g. $1A632, $1A64A (after a spawn + Op4_ChildSaveDrawPos); 6 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildJitterXY:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,$2E(a5)
; ---------------------------------------------------------------------------
; Op4_ChildJitterY - a4 op: moves the child's y $32(a5) by a random [-w/2, w/2) px
; In:  a4 = script: word w; a5 = child
; Out: NEXT
; Evidence: label-only entry inside Op4_ChildJitterXY (computed-jump target in 1 corpus run, fuzz); script use $4954A
; ---------------------------------------------------------------------------
Op4_ChildJitterY:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,$32(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
