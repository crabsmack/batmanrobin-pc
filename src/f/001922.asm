; ---------------------------------------------------------------------------
; Mot_SeekScreenX - op tail of Op2_SeekScreenX/X4: compares the object's screen x (x $3E(a6) - g_CameraX) with the target x $14(a3): more than d1 left of it -> Op2_CruiseRight, d1 or more right of it -> Op2_CruiseLeft, else the x target speed is cleared
; In:  d1 = tolerance; a2 = script; a3 = motion block, a6 = object
; Out: NEXT
; Evidence: falls in from $191C, bra from $1918; branches to $180E/$1820; executed in 11 runs incl. the TAS (frames 77185-130448)
; ---------------------------------------------------------------------------
Mot_SeekScreenX:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	sub.w	$14(a3),d0
	add.w	d1,d0
	blt.w	Op2_CruiseRight
	sub.w	d1,d0
	sub.w	d1,d0
	bge.w	Op2_CruiseLeft
	clr.l	$24(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SeekScreenXOrBranch - a2 op: steers toward the target screen x $14(a3) like Op2_SeekScreenX, and branches once within the given tolerance (x target speed cleared)
; In:  a2 = script: word tolerance, word branch offset; a3 = motion block
; Out: branch (arrived) or NEXT past the offset (still moving)
; Evidence: move.w (a2)+,d1 then bra $194E; 12 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenXOrBranch:
	move.w	(a2)+,d1
	bra.s	Mot_SeekScreenXOrBranch
; ---------------------------------------------------------------------------
; Op2_SeekScreenX8OrBranch - a2 op: Mot_SeekScreenXOrBranch with an 8-pixel tolerance
; In:  a2 = script: word branch offset; a3 = motion block
; Out: branch when arrived, else NEXT
; Evidence: moveq #8,d1 then bra $194E; 14 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenX8OrBranch:
	moveq	#$8,d1
	bra.s	Mot_SeekScreenXOrBranch
