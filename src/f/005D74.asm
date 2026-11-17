; ---------------------------------------------------------------------------
; Op4_ChildJitterHeading - a4 op: turns the child's heading $2A(a5) by a random [-w/2, w/2) units ($400 per turn)
; In:  a4 = script: word w; a5 = child
; Out: NEXT
; Evidence: Rand_Next * w added to $2A(a5); 61 a4-script uses, e.g. $25B72, $25B90 (spawn, random direction, then Op4_ChildAddVelRadius); 150 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildJitterHeading:
	bsr.w	Rand_Next
	muls.w	(a4)+,d0
	add.l	d0,$2A(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildScatterHeading - a4 op: turns the child's heading $2A(a5) by base + a random [0, w/2) units, clockwise or anticlockwise at random
; In:  a4 = script: word base (whole units), word w; a5 = child
; Out: NEXT
; Evidence: d1 = base<<16, negated when the signed Rand_Next * w part is negative, both added to $2A(a5); script use $26AD2; 3 corpus runs (fuzz only)
; ---------------------------------------------------------------------------
Op4_ChildScatterHeading:
	bsr.w	Rand_Next
	moveq	#$0,d1
	move.w	(a4)+,d1
	swap	d1
	muls.w	(a4)+,d0
	bpl.s	loc_005D92
	neg.l	d1
loc_005D92:
	add.l	d1,d0
	add.l	d0,$2A(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildJitterVelX - a4 op: adds a random [-w/2, w/2) (8.8 px/frame) to the child's x velocity $3E(a5)
; In:  a4 = script: word w (8.8); a5 = child
; Out: NEXT
; Evidence: Rand_Next asr.w #4 * w, asr.l #4, added to $3E(a5); script uses $1A638, $1A69E, $1A6CE; 17 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildJitterVelX:
	bsr.w	Rand_Next
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	add.l	d0,$3E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ChildScatterVelX - a4 op: kicks the child's x velocity $3E(a5) by base + a random [0, w/2) (8.8), left or right at random
; In:  a4 = script: word base (8.8), word w (8.8); a5 = child
; Out: NEXT
; Evidence: the Op4_ScatterVelX code on $3E(a5); 23 a4-script uses, e.g. $1A3B0 (`5DAE 0100 FF00`), $1A650; 22 corpus runs
; ---------------------------------------------------------------------------
Op4_ChildScatterVelX:
	bsr.w	Rand_Next
	move.w	(a4)+,d1
	ext.l	d1
	asl.l	#$8,d1
	asr.w	#$4,d0
	muls.w	(a4)+,d0
	asr.l	#$4,d0
	bpl.s	loc_005DC2
	neg.l	d1
loc_005DC2:
	add.l	d1,d0
	add.l	d0,$3E(a5)
	movea.w	(a4)+,a0
	jmp	(a0)
