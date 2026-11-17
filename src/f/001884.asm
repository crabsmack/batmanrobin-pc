; ---------------------------------------------------------------------------
; Op2_StopX - a2 op: stops horizontal motion at once: x target speed $24 and x velocity $18 of a3 set to 0
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: two clr.l; 111 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_StopX:
	clr.l	$24(a3)
	clr.l	$18(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ArcX - a2 op: the x-axis form of Op2_Jump: x acceleration $20 = Mot_JumpGravity(distance, time) made positive, x velocity $18 = the matching launch speed (Mot_JumpLaunchSpeed), x target speed $24 = the x cruise speed
; In:  a2 = script: word distance, word time; a3 = motion block
; Out: NEXT
; Evidence: same helper pair as Op2_Jump on the x fields (the neg of $24 for a negative distance is overwritten by the final move); 52 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_ArcX:
	bsr.w	Mot_JumpGravity
	move.l	d1,$20(a3)
	bpl.s	loc_0018A2
	neg.l	$20(a3)
	neg.l	$24(a3)
loc_0018A2:
	bsr.w	Mot_JumpLaunchSpeed
	move.l	d1,$18(a3)
	move.l	$30(a3),$24(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BurstX - a2 op: gives a short horizontal burst: x velocity = 2 * word px per run in the facing direction, x target speed 0 and x acceleration |word|, so it brakes to a stop
; In:  a2 = script: word speed (whole pixels); a3 = motion block, a5 = sprite block (bit 3 of $38 = flipped)
; Out: NEXT
; Evidence: word swapped to 16.16; $24 = 0, $20 = |v|, $18 = 2v (negated when flipped); 20 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BurstX:
	moveq	#$0,d0
	move.l	d0,$24(a3)
	move.w	(a2)+,d0
	swap	d0
	move.l	d0,d1
	bpl.s	loc_0018C4
	neg.l	d0
loc_0018C4:
	move.l	d0,$20(a3)
	add.l	d1,d1
	btst	#$3,$38(a5)
	beq.s	loc_0018D4
	neg.l	d1
loc_0018D4:
	move.l	d1,$18(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BurstY - a2 op: gives a short vertical burst: y velocity = 2 * word px per run (sign as given), y target speed 0 and y acceleration |word|
; In:  a2 = script: word speed (whole pixels, negative = up); a3 = motion block
; Out: NEXT
; Evidence: as Op2_BurstX on $2C/$28/$1C, without the facing test; 5 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BurstY:
	moveq	#$0,d0
	move.l	d0,$2C(a3)
	move.w	(a2)+,d0
	swap	d0
	move.l	d0,d1
	bpl.s	loc_0018EC
	neg.l	d0
loc_0018EC:
	move.l	d0,$28(a3)
	add.l	d1,d1
	move.l	d1,$1C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FaceTargetSpeedX - a2 op: turns the sprite to face along the x target speed: H flip (bit 3 of $38(a5)) cleared when $24(a3) > 0, set when < 0, unchanged when 0
; In:  a2 = script (no operand); a3 = motion block, a5 = sprite block
; Out: NEXT
; Evidence: tst.w $24(a3) then bclr/bset #3,$38(a5); 14 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_FaceTargetSpeedX:
	tst.w	$24(a3)
	beq.w	VM2_Next
	bmi.s	loc_00190E
	bclr	#$3,$38(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_00190E:
	bset	#$3,$38(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SeekScreenX - a2 op: steers horizontally toward the target screen x $14(a3), stopping within the given tolerance (Mot_SeekScreenX)
; In:  a2 = script: word tolerance (pixels); a3 = motion block
; Out: NEXT
; Evidence: move.w (a2)+,d1 then bra $1922; 4 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenX:
	move.w	(a2)+,d1
	bra.s	Mot_SeekScreenX
; ---------------------------------------------------------------------------
; Op2_SeekScreenX4 - a2 op: Mot_SeekScreenX with a 4-pixel tolerance
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: moveq #4,d1 then bra $1922; 8 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenX4:
	moveq	#$4,d1
	bra.s	Mot_SeekScreenX
