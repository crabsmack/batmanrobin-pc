; ---------------------------------------------------------------------------
; Mot_SeekWorldXOrBranch - op tail: like Mot_SeekScreenXOrBranch but compares the world x $3E(a6) with the target x $14(a3) (no camera offset)
; In:  d1 = tolerance; a2 = script: word branch offset; a3 = motion block, a6 = object
; Out: branch (within d1, x target speed cleared) or NEXT after cruising right/left
; Evidence: no g_CameraX subtraction, otherwise $194E's code; executed in 14 runs incl. the TAS (6895 TAS frames)
; ---------------------------------------------------------------------------
Mot_SeekWorldXOrBranch:
	move.w	$3E(a6),d0
	sub.w	$14(a3),d0
	add.w	d1,d0
	blt.s	loc_001A14
	sub.w	d1,d0
	sub.w	d1,d0
	bge.s	loc_001A0E
	clr.l	$24(a3)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001A0E:
	addq.l	#$2,a2
	bra.w	Op2_CruiseLeft
loc_001A14:
	addq.l	#$2,a2
	bra.w	Op2_CruiseRight
; ---------------------------------------------------------------------------
; Op2_SeekWorldXRange - a2 op: keeps the object's world x $3E(a6) inside [lo, hi] by cruising right/left, clearing the x target speed inside
; In:  a2 = script: word lo, word hi (world pixels); a3 = motion block
; Out: NEXT
; Evidence: as Op2_SeekScreenXRange without the camera; 1 corpus run (the TAS)
; ---------------------------------------------------------------------------
Op2_SeekWorldXRange:
	move.w	$3E(a6),d0
	cmp.w	(a2)+,d0
	blt.s	loc_001A30
	cmp.w	(a2)+,d0
	bgt.w	Op2_CruiseLeft
	clr.l	$24(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001A30:
	addq.l	#$2,a2
	bra.w	Op2_CruiseRight
