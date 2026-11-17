; ---------------------------------------------------------------------------
; Op2_AddTargetY - a2 op: moves the motion target point's y $16(a3) by a signed word
; In:  a2 = script: word dy; a3 = motion block
; Out: NEXT
; Evidence: add.w to $16(a3), the y that the Y seek ops ($1A62-$1B5A) compare against; entered by dispatch in 3 corpus runs incl. the TAS (frames 116492-118792)
; ---------------------------------------------------------------------------
Op2_AddTargetY:
	move.w	(a2)+,d0
	add.w	d0,$16(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AddTargetX - a2 op: moves the motion target point's x $14(a3) by a signed word
; In:  a2 = script: word dx; a3 = motion block
; Out: NEXT
; Evidence: add.w to $14(a3), the x that the X seek ops ($1922-$1A1A) compare against; entered by dispatch in 2 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_AddTargetX:
	move.w	(a2)+,d0
	add.w	d0,$14(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_TargetSelf - a2 op: makes the object's own world position (x $3E, y $42 of a6) the motion target point $14/$16(a3), without Mot_SetTarget's direction bits
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: two word moves; entered by dispatch in 64 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_TargetSelf:
	move.w	$3E(a6),$14(a3)
	move.w	$42(a6),$16(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
