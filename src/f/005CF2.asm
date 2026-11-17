; ---------------------------------------------------------------------------
; Op4_ChildJitterX - a4 op: moves the last spawned child's x $2E(a5) by a random [-w/2, w/2) px
; In:  a4 = script: word w; a5 = child
; Out: NEXT
; Evidence: Rand_Next * w added to $2E(a5); script uses $1A390, $1ECAC, $1ECE4; 7 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildJitterX:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,$2E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
