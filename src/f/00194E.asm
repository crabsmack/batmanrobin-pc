; ---------------------------------------------------------------------------
; Mot_SeekScreenXOrBranch - op tail of Op2_SeekScreenX(8)OrBranch: screen x vs target x $14(a3) with tolerance d1; outside it skips the offset word and cruises right/left, inside it clears the x target speed and takes the branch
; In:  d1 = tolerance; a2 = script: word branch offset; a3 = motion block, a6 = object
; Out: branch or NEXT
; Evidence: adda.w (a2),a2 on arrival, addq #2 then bra $180E/$1820 otherwise; 25 runs incl. the TAS (6039 TAS frames from 4540)
; ---------------------------------------------------------------------------
Mot_SeekScreenXOrBranch:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	sub.w	$14(a3),d0
	add.w	d1,d0
	blt.s	loc_001974
	sub.w	d1,d0
	sub.w	d1,d0
	bge.s	loc_00196E
	clr.l	$24(a3)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_00196E:
	addq.l	#$2,a2
	bra.w	Op2_CruiseLeft
loc_001974:
	addq.l	#$2,a2
	bra.w	Op2_CruiseRight
; ---------------------------------------------------------------------------
; Op2_SeekScreenXRange - a2 op: keeps the object's screen x inside [lo, hi]: left of lo -> Op2_CruiseRight, right of hi -> Op2_CruiseLeft, inside -> x target speed cleared
; In:  a2 = script: word lo, word hi (screen pixels); a3 = motion block, a6 = object
; Out: NEXT
; Evidence: x $3E(a6) - g_CameraX compared with both words; 7 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenXRange:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	cmp.w	(a2)+,d0
	blt.s	loc_001994
	cmp.w	(a2)+,d0
	bgt.w	Op2_CruiseLeft
	clr.l	$24(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001994:
	addq.l	#$2,a2
	bra.w	Op2_CruiseRight
; ---------------------------------------------------------------------------
; Op2_SeekScreenXRangeOrBranch - a2 op: as Op2_SeekScreenXRange, and branches when the screen x is already inside [lo, hi]
; In:  a2 = script: word lo, word hi, word branch offset; a3 = motion block
; Out: branch (inside) or NEXT past the offset
; Evidence: adda.w (a2),a2 inside, addq #2/#4 then $1820/$180E outside; 14 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenXRangeOrBranch:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	cmp.w	(a2)+,d0
	blt.s	loc_0019BA
	cmp.w	(a2)+,d0
	bgt.s	loc_0019B4
	clr.l	$24(a3)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_0019B4:
	addq.l	#$2,a2
	bra.w	Op2_CruiseLeft
loc_0019BA:
	addq.l	#$4,a2
	bra.w	Op2_CruiseRight
