; ---------------------------------------------------------------------------
; Mot_SeekWorldYOrBranch - op tail: like Mot_SeekScreenYOrBranch but compares the world y $42(a6) with the target y $16(a3)
; In:  d1 = tolerance; a2 = script: word branch offset; a3 = motion block, a6 = object
; Out: branch (within d1, y target speed cleared) or NEXT after cruising down/up
; Evidence: no g_CameraY subtraction; executed in 11 runs incl. the TAS (1781 TAS frames)
; ---------------------------------------------------------------------------
Mot_SeekWorldYOrBranch:
	move.w	$42(a6),d0
	sub.w	$16(a3),d0
	add.w	d1,d0
	blt.s	loc_001B54
	sub.w	d1,d0
	sub.w	d1,d0
	bge.s	loc_001B4E
	clr.l	$2C(a3)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001B4E:
	addq.l	#$2,a2
	bra.w	Op2_CruiseUp
loc_001B54:
	addq.l	#$2,a2
	bra.w	Op2_CruiseDown
; ---------------------------------------------------------------------------
; Op2_SeekWorldYRange - a2 op: keeps the object's world y $42(a6) inside [lo, hi] by cruising down/up, clearing the y target speed inside
; In:  a2 = script: word lo, word hi (world pixels); a3 = motion block
; Out: NEXT
; Evidence: as Op2_SeekScreenYRange without the camera; 3 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekWorldYRange:
	move.w	$42(a6),d0
	cmp.w	(a2)+,d0
	blt.s	loc_001B70
	cmp.w	(a2)+,d0
	bgt.w	Op2_CruiseUp
	clr.l	$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001B70:
	addq.l	#$2,a2
	bra.w	Op2_CruiseDown
