; ---------------------------------------------------------------------------
; Op2_BranchPastTarget - a2 op: branches when the object has reached or passed its motion target along the dominant axis in its direction of travel (bits 3/4 of (a3) from Mot_SetTarget), re-deriving the heading from the velocity as it does
; In:  a2 = script: word offset
; Out: branch and Op2_HeadingFromVel; not there yet: NEXT after the offset
; Evidence: compares $14(a3) - x or $16(a3) - y with the direction bit; 6.1k TAS frames
; ---------------------------------------------------------------------------
Op2_BranchPastTarget:
	btst	#$3,(a3)
	bne.s	loc_00147A
	move.w	$14(a3),d0
	sub.w	$3E(a6),d0
	bpl.s	loc_00146C
	btst	#$4,(a3)
	beq.s	loc_001474
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_00146C:
	btst	#$4,(a3)
	beq.w	Op2_SkipWord
loc_001474:
	adda.w	(a2),a2
	bra.w	Op2_HeadingFromVel
loc_00147A:
	move.w	$16(a3),d0
	sub.w	$42(a6),d0
	bpl.s	loc_001490
	btst	#$4,(a3)
	beq.s	loc_001474
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001490:
	btst	#$4,(a3)
	bne.s	loc_001474
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
