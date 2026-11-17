; ---------------------------------------------------------------------------
; Op2_ClearTargetSpeedY - a2 op: sets the y target speed $2C(a3) to 0, so the steering tail brakes the y velocity by the y acceleration
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: clr.l $2C(a3); 100 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_ClearTargetSpeedY:
	clr.l	$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StopY - a2 op: stops vertical motion at once: y target speed $2C and y velocity $1C of a3 set to 0
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: two clr.l; 131 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_StopY:
	clr.l	$2C(a3)
	clr.l	$1C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetJumpGravity - a2 op: sets only the y acceleration $28(a3) to the gravity of a jump of the given height and time (Mot_JumpGravity), leaving the velocity alone
; In:  a2 = script: word height, word time; a3 = motion block
; Out: NEXT
; Evidence: bsr Mot_JumpGravity, move.l d1,$28(a3); 89 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetJumpGravity:
	bsr.w	Mot_JumpGravity
	move.l	d1,$28(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchIfNotRising - a2 op: branches when the y velocity $1C(a3) is >= 0 (falling or still), else skips the offset
; In:  a2 = script: word branch offset; a3 = motion block
; Out: branch or Op2_SkipWord
; Evidence: tst.l $1C(a3), bmi Op2_SkipWord; 94 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchIfNotRising:
	tst.l	$1C(a3)
	bmi.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchIfRising - a2 op: branches when the y velocity $1C(a3) is negative (moving up), else skips the offset
; In:  a2 = script: word branch offset; a3 = motion block
; Out: branch or Op2_SkipWord
; Evidence: tst.l $1C(a3), bpl Op2_SkipWord; 33 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchIfRising:
	tst.l	$1C(a3)
	bpl.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_JumpAnimRandom - a2 op: Op2_JumpAnim with a random height: height = random below the first word + the second word
; In:  a2 = script: word height range, word base height, word time; a3/a5 = motion and sprite blocks
; Out: NEXT
; Evidence: Rand_Below (unsigned random * d0 >> 16) + base, Mot_JumpGravityD1, then joins $1C90; 3 fuzz corpus runs only (not the TAS)
; ---------------------------------------------------------------------------
Op2_JumpAnimRandom:
	move.w	(a2)+,d0
	bsr.w	Rand_Below
	add.w	(a2)+,d0
	andi.w	#$FFFF,d0
	move.l	d0,d1
	bsr.w	Mot_JumpGravityD1
	bra.s	loc_001C90
; ---------------------------------------------------------------------------
; Op2_JumpAnim - a2 op: starts a jump that peaks height px up after time units: y gravity $28 = Mot_JumpGravity, y target speed $2C = the y cruise speed (fall limit; both negated for a negative height), adds the launch speed to the y velocity $1C, and sets the sprite's velocity-to-frame mapping ($32(a5) = -launch speed, $36(a5) = that / 50)
; In:  a2 = script: word height, word time; a3 = motion block, a4 = speed block, a5 = sprite block
; Out: NEXT
; Evidence: the frame ops $3EE2/$3F1C pick a case from (y velocity + $32(a5)) / $36(a5), 0-100 from launch to the matching fall speed (inferred from the arithmetic); 105 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_JumpAnim:
	bsr.w	Mot_JumpGravity
loc_001C90:
	move.l	$34(a3),$2C(a3)
	move.l	d1,$28(a3)
	bpl.s	loc_001CA4
	neg.l	$28(a3)
	neg.l	$2C(a3)
loc_001CA4:
	bsr.w	Mot_JumpLaunchSpeed
	add.l	d1,$1C(a3)
	neg.l	d1
	move.l	d1,$32(a5)
	divs.w	#$32,d1
	move.w	d1,$36(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_Jump - a2 op: starts a jump that peaks height px up after time units: y gravity $28 = Mot_JumpGravity, y velocity $1C = the launch speed (Mot_JumpLaunchSpeed), y target speed $2C = the y cruise speed (fall limit)
; In:  a2 = script: word height, word time; a3 = motion block, a4 = speed block
; Out: NEXT
; Evidence: sets rather than adds the velocity and leaves the sprite mapping alone (cf. Op2_JumpAnim); 66 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_Jump:
	bsr.w	Mot_JumpGravity
	move.l	d1,$28(a3)
	bsr.w	Mot_JumpLaunchSpeed
	move.l	d1,$1C(a3)
	move.l	$34(a3),$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FallAnim - a2 op: starts a fall from rest with the gravity of a jump (height, time): y moved down 1 px, y gravity $28 set, y velocity 0, y target speed = y cruise speed, sprite velocity-to-frame mapping set as for the matching jump
; In:  a2 = script: word height, word time; a3/a4/a5 = motion, speed and sprite blocks, a6 = object
; Out: NEXT
; Evidence: addq.w #1,$42(a6), clr.l $1C(a3), $32/$36(a5) from the launch speed; 9 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_FallAnim:
	addq.w	#$1,$42(a6)
; ---------------------------------------------------------------------------
; Op2_FallAnimInPlace - a2 op: Op2_FallAnim without its 1-pixel nudge down: a fall from rest with the gravity of a jump (height, time) and the matching sprite frame mapping
; In:  a2 = script: word height, word time; a3/a4/a5 = motion, speed and sprite blocks
; Out: NEXT
; Evidence: a computed-jump target in the corpus (script handler word $1CDC: the street clowns $1B634, gunmen $1BFB8, bomb rabbits $35CB8 fall in with it), ~3.3k call-site log entries
; ---------------------------------------------------------------------------
Op2_FallAnimInPlace:
	bsr.w	Mot_JumpGravity
	move.l	d1,$28(a3)
	bsr.w	Mot_JumpLaunchSpeed
	clr.l	$1C(a3)
	neg.l	d1
	move.l	d1,$32(a5)
	divs.w	#$32,d1
	move.w	d1,$36(a5)
	move.l	$34(a3),$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AnimByCruiseY - a2 op: sets the sprite's velocity-to-frame mapping for a straight move: divisor $36(a5) = y cruise speed / 100 (at least 1), bias $32(a5) = 0
; In:  a2 = script (no operand); a3 = motion block, a5 = sprite block
; Out: NEXT
; Evidence: divs.w #$64 of $34(a3); read by the frame ops $3EE2/$3F1C; 43 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_AnimByCruiseY:
	move.l	$34(a3),d0
	divs.w	#$64,d0
	bne.s	loc_001D10
	moveq	#$1,d0
loc_001D10:
	move.w	d0,$36(a5)
	clr.l	$32(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AnimByCruiseX - a2 op: sets the sprite's velocity-to-frame mapping from the x cruise speed: divisor $36(a5) = $30(a3) / 100 (at least 1), bias $32(a5) = 0
; In:  a2 = script (no operand); a3 = motion block, a5 = sprite block
; Out: NEXT
; Evidence: divs.w #$64 of $30(a3); read by $3F1C (x velocity form); 25 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_AnimByCruiseX:
	move.l	$30(a3),d0
	divs.w	#$64,d0
	bne.s	loc_001D28
	moveq	#$1,d0
loc_001D28:
	move.w	d0,$36(a5)
	clr.l	$32(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BounceY - a2 op: bounces after a landing: when half the last y velocity ($3C(a3) / 2) exceeds the y gravity $28, the y velocity becomes minus that (up at half speed); otherwise vertical motion stops
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: $38/$3C(a3) is the copy of the velocity the steering tail $4B8A writes before terrain contact clears $18/$1C; 13 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BounceY:
	move.l	$3C(a3),d0
	asr.l	#$1,d0
	cmp.l	$28(a3),d0
	bgt.s	loc_001D4C
	clr.l	$1C(a3)
	clr.l	$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001D4C:
	neg.l	d0
	move.l	d0,$1C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BounceYOrBranch - a2 op: bounces like Op2_BounceY while half the last y velocity exceeds $6000 (0.375 px per run) and continues; once the bounce has died out it stops vertical motion and branches
; In:  a2 = script: word branch offset; a3 = motion block
; Out: branch (stopped) or NEXT past the offset (bounced)
; Evidence: cmpi.l #$6000; adda.w (a2),a2 on the stop path; 77 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BounceYOrBranch:
	move.l	$3C(a3),d0
	asr.l	#$1,d0
	cmpi.l	#$6000,d0
	bgt.s	loc_001D72
	clr.l	$1C(a3)
	clr.l	$2C(a3)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_001D72:
	neg.l	d0
	move.l	d0,$1C(a3)
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
