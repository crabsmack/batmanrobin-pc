; ---------------------------------------------------------------------------
; Op2_ApplyMoveLandAtY - a2 op: adds this frame's move d1/d2 to x/y and lands on a floor at a word y: there y is clamped, the motion block's on-ground word $54(a3) set and its y velocity $1C(a3) cleared; off it $54(a3) is cleared
; In:  a2 = script: word floor y (whole pixels); d1/d2 = move (16.16); a3 = motion block
; Out: $3E/$42(a6) moved; $54(a3) = 1 and $1C(a3) = 0 on the floor, else $54(a3) = 0; NEXT
; Evidence: add.l d1,$3E(a6) / cmp.l with the floor before and after add.l d2,$42(a6) / move.w #1,$54(a3) / clr.l $1C(a3) or clr.w $54(a3); the op after Op2_ApplyMoveBranchAtY in the same span; never run in the corpus
; ---------------------------------------------------------------------------
Op2_ApplyMoveLandAtY:
	add.l	d1,$3E(a6)
	moveq	#$0,d0
	move.w	(a2)+,d0
	swap	d0
	cmp.l	$42(a6),d0
	bge.s	loc_004CC8
	add.l	d2,$42(a6)
	cmp.l	$42(a6),d0
	blt.w	loc_004CD2
loc_004CB6:
	move.l	d0,$42(a6)
	move.w	#$1,$54(a3)
	clr.l	$1C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_004CC8:
	add.l	d2,$42(a6)
	cmp.l	$42(a6),d0
	ble.s	loc_004CB6
loc_004CD2:
	clr.w	$54(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
