; ---------------------------------------------------------------------------
; Mot_SeekScreenYOrBranch - op tail of Op2_SeekScreenY(8)OrBranch: screen y vs target y $16(a3) with tolerance d1; outside it skips the offset and cruises down/up, inside it clears the y target speed and takes the branch
; In:  d1 = tolerance; a2 = script: word branch offset; a3 = motion block, a6 = object
; Out: branch or NEXT
; Evidence: adda.w (a2),a2 on arrival, else bra $1BFE/$1BEC; executed in 15 runs incl. the TAS (5656 TAS frames, 11291-35416)
; ---------------------------------------------------------------------------
Mot_SeekScreenYOrBranch:
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	sub.w	$16(a3),d0
	add.w	d1,d0
	blt.s	loc_001AB4
	sub.w	d1,d0
	sub.w	d1,d0
	bge.s	loc_001AAE
	clr.l	$2C(a3)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001AAE:
	addq.l	#$2,a2
	bra.w	Op2_CruiseUp
loc_001AB4:
	addq.l	#$2,a2
	bra.w	Op2_CruiseDown
; ---------------------------------------------------------------------------
; Op2_SeekScreenYRange - a2 op: keeps the object's screen y inside [lo, hi]: above lo -> Op2_CruiseDown, below hi -> Op2_CruiseUp, inside -> y target speed cleared
; In:  a2 = script: word lo, word hi (screen pixels); a3 = motion block, a6 = object
; Out: NEXT
; Evidence: y $42(a6) - g_CameraY compared with both words; 7 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenYRange:
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	cmp.w	(a2)+,d0
	blt.s	loc_001AD4
	cmp.w	(a2)+,d0
	bgt.w	Op2_CruiseUp
	clr.l	$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001AD4:
	addq.l	#$2,a2
	bra.w	Op2_CruiseDown
; ---------------------------------------------------------------------------
; Op2_SeekScreenYRangeOrBranch - a2 op: as Op2_SeekScreenYRange, and branches when the screen y is already inside [lo, hi]
; In:  a2 = script: word lo, word hi, word branch offset; a3 = motion block
; Out: branch (inside) or NEXT past the offset
; Evidence: adda.w (a2),a2 inside; 3 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenYRangeOrBranch:
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	cmp.w	(a2)+,d0
	blt.s	loc_001AFA
	cmp.w	(a2)+,d0
	bgt.s	loc_001AF4
	clr.l	$2C(a3)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001AF4:
	addq.l	#$2,a2
	bra.w	Op2_CruiseUp
loc_001AFA:
	addq.l	#$4,a2
	bra.w	Op2_CruiseDown
