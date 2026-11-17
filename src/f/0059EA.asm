; ---------------------------------------------------------------------------
; Op4_JitterHeading - a4 op: turns the object's heading $2A by a random [-w/2, w/2) units ($400 per turn)
; In:  a4 = script: word w
; Out: NEXT
; Evidence: Rand_Next * w added to $2A(a6); script uses $26AB0, $2981A, $3601E; 33 corpus runs
; ---------------------------------------------------------------------------
Op4_JitterHeading:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,$2A(a6)
	movea.w	(a4)+,a0
	jmp	(a0)
