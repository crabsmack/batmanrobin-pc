; ---------------------------------------------------------------------------
; Mot_SeekScreenY - op tail of Op2_SeekScreenY4: compares the screen y (y $42(a6) - g_CameraY) with the target y $16(a3): above it by more than d1 -> Op2_CruiseDown, d1 or more below -> Op2_CruiseUp, else the y target speed is cleared
; In:  d1 = tolerance; a2 = script; a3 = motion block, a6 = object
; Out: NEXT
; Evidence: branches to $1BEC/$1BFE, clr.l $2C(a3); executed in 18 runs incl. the TAS (1656 TAS frames)
; ---------------------------------------------------------------------------
Mot_SeekScreenY:
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	sub.w	$16(a3),d0
	add.w	d1,d0
	blt.w	Op2_CruiseDown
	sub.w	d1,d0
	sub.w	d1,d0
	bge.w	Op2_CruiseUp
	clr.l	$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SeekScreenYOrBranch - a2 op: steers toward the target screen y $16(a3) and branches once within the given tolerance
; In:  a2 = script: word tolerance, word branch offset; a3 = motion block
; Out: branch (arrived) or NEXT
; Evidence: move.w (a2)+,d1 then bra $1A8E; 8 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenYOrBranch:
	move.w	(a2)+,d1
	bra.s	Mot_SeekScreenYOrBranch
; ---------------------------------------------------------------------------
; Op2_SeekScreenY8OrBranch - a2 op: Mot_SeekScreenYOrBranch with an 8-pixel tolerance
; In:  a2 = script: word branch offset; a3 = motion block
; Out: branch when arrived, else NEXT
; Evidence: moveq #8,d1 then bra $1A8E; 14 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenY8OrBranch:
	moveq	#$8,d1
	bra.s	Mot_SeekScreenYOrBranch
