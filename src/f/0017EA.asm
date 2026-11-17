; ---------------------------------------------------------------------------
; Op2_CruiseNow - a2 op: sets both velocities and both target speeds to the cruise speeds at once ($18/$1C and $24/$2C of a3 = $30/$34)
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: four long moves; 1 corpus run (the TAS, frame 79014)
; ---------------------------------------------------------------------------
Op2_CruiseNow:
	move.l	$34(a3),$1C(a3)
	move.l	$30(a3),$18(a3)
; ---------------------------------------------------------------------------
; Op2_CruiseTargets - a2 op: sets both target speeds (x $24, y $2C of a3) to the cruise speeds $30/$34 (Op2_CruiseNow past its velocity moves), so the steering accelerates toward them
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: a computed-jump target in the corpus: NEXT dispatches from $13C8, $347E, $1552, $BE4 and others, ~16k call-site log entries
; ---------------------------------------------------------------------------
Op2_CruiseTargets:
	move.l	$34(a3),$2C(a3)
; ---------------------------------------------------------------------------
; Op2_CruiseTargetX - a2 op: sets the x target speed $24(a3) to the x cruise speed $30(a3) (signed, unlike Op2_CruiseRight/Left)
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: a computed-jump target in the corpus: the NEXT of Op2_SetCruiseX ($17B6), 5 call-site log entries
; ---------------------------------------------------------------------------
Op2_CruiseTargetX:
	move.l	$30(a3),$24(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_CruiseForward - a2 op: sets the x target speed to the x cruise speed's magnitude in the facing direction: right (Op2_CruiseRight) unless the sprite is H-flipped (Op2_CruiseLeft)
; In:  a2 = script (no operand); a3 = motion block, a5 = sprite block
; Out: NEXT
; Evidence: btst #3,$38(a5) chooses $1820 or $180E; 88 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_CruiseForward:
	btst	#$3,$38(a5)
	bne.s	Op2_CruiseLeft
; ---------------------------------------------------------------------------
; Op2_CruiseRight - a2 op: sets the x target speed $24(a3) to +|x cruise speed $30(a3)| (accelerate rightward); also the 'target is to the right' exit of the X seek ops
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: move.l $30,$24 then neg when negative; branched to from Mot_SeekScreenX/XOrBranch/WorldXOrBranch; entered by dispatch in 73 runs
; ---------------------------------------------------------------------------
Op2_CruiseRight:
	move.l	$30(a3),$24(a3)
	bpl.w	VM2_Next
	neg.l	$24(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_CruiseLeft - a2 op: sets the x target speed $24(a3) to -|x cruise speed $30(a3)| (accelerate leftward); also the 'target is to the left' exit of the X seek ops
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: move.l $30,$24 then neg when positive; branched to from the X seek ops; entered by dispatch in 90 runs
; ---------------------------------------------------------------------------
Op2_CruiseLeft:
	move.l	$30(a3),$24(a3)
	bmi.w	VM2_Next
	neg.l	$24(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_CruiseRightNow - a2 op: sets both the x target speed and the x velocity to +|x cruise speed| (moves right at full speed at once)
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: |$30(a3)| to $24 and $18; 1 corpus run (the TAS)
; ---------------------------------------------------------------------------
Op2_CruiseRightNow:
	move.l	$30(a3),d0
	bpl.s	loc_00183A
	neg.l	d0
loc_00183A:
	move.l	d0,$24(a3)
	move.l	d0,$18(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
