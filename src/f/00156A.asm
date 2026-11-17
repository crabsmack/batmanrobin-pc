; ---------------------------------------------------------------------------
; Mot_TurnTowardHeading - turns the current heading $C(a3) (16.16, 1024 per turn) toward the target heading $A(a3) by the turn rate $10(a3), the shorter way round, landing on the target instead of overshooting it
; In:  a3 = motion block
; Out: $C(a3) updated; d0-d2 clobbered
; Evidence: differences masked to $3FFFFFF with $2000000 as the half turn; callers Op2_TurnSteer and Op2_TurnHeading; 15.8k TAS frames
; ---------------------------------------------------------------------------
Mot_TurnTowardHeading:
	moveq	#$0,d0
	move.w	$A(a3),d0
	swap	d0
	move.l	$C(a3),d1
	move.l	d0,d2
	sub.l	d1,d2
	andi.l	#$3FFFFFF,d2
	cmpi.l	#$2000000,d2
	bgt.s	loc_0015A0
	add.l	$10(a3),d1
	move.l	d0,d2
	sub.l	d1,d2
	andi.l	#$3FFFFFF,d2
	cmpi.l	#$2000000,d2
	blt.s	loc_0015B8
	bra.s	loc_0015B6
loc_0015A0:
	sub.l	$10(a3),d1
	move.l	d0,d2
	sub.l	d1,d2
	andi.l	#$3FFFFFF,d2
	cmpi.l	#$2000000,d2
	bgt.s	loc_0015B8
loc_0015B6:
	move.l	d0,d1
loc_0015B8:
	move.l	d1,$C(a3)
	rts
; ---------------------------------------------------------------------------
; Mot_SteerToHeading - op tail: accelerates the velocity $18/$1C(a3) toward the target speeds $24/$2C turned by the current heading, by steps $20/$28 (via $4B8A); with both target speeds 0 it brakes toward 0; while the stun count $1(a3) is set it only counts down and decays the impulse $2/$6(a3)
; In:  a2 = script, a3 = motion block
; Out: NEXT (end of $4B74's code); velocity also copied to $38/$3C(a3)
; Evidence: reached by bra from Op2_SnapHeadingSteer/Op2_TurnSteer; $4B8A moves d1/d2 toward d4/d6 by d3/d5 and stores them at $18 and $38; executed in the TAS
; ---------------------------------------------------------------------------
Mot_SteerToHeading:
	tst.b	$1(a3)
	beq.s	loc_0015D0
	subq.b	#$1,$1(a3)
	moveq	#$0,d1
	moveq	#$0,d2
	bra.w	Op2_StepVelocity_Impulse
loc_0015D0:
	movem.l	$18(a3),d1-d6
	move.w	$C(a3),d0
	andi.w	#$3FE,d0
	lea.l	(Tbl_059B22).l,a0
	adda.w	d0,a0
	move.l	d4,d0
	or.l	d6,d0
	beq.s	loc_0015FE
	asr.l	#$8,d4
	asr.l	#$8,d6
	muls.w	$100(a0),d4
	muls.w	(a0),d6
	asr.l	#$7,d4
	asr.l	#$7,d6
	bra.w	Op2_StepVelocity_Steer
loc_0015FE:
	asr.l	#$8,d3
	asr.l	#$8,d5
	muls.w	$100(a0),d3
	muls.w	(a0),d5
	asr.l	#$7,d3
	bpl.s	loc_00160E
	neg.l	d3
loc_00160E:
	asr.l	#$7,d5
	bpl.w	Op2_StepVelocity_Steer
	neg.l	d5
	bra.w	Op2_StepVelocity_Steer
; ---------------------------------------------------------------------------
; Op2_SetPos - a2 op: places the object at the world point (x, y) words, fractions cleared
; In:  a2 = script: words x, y
; Out: NEXT
; Evidence: writes $3E/$42(a6) and clears $40/$44; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetPos:
	move.w	(a2)+,$3E(a6)
	clr.w	$40(a6)
; ---------------------------------------------------------------------------
; Op2_SetY - a2 op: sets the object's world y ($42(a6)) from a word, fraction $44(a6) cleared: Op2_SetPos past its x
; In:  a2 = script: word y
; Out: NEXT
; Evidence: move.w (a2)+,$42(a6) / clr.w $44(a6); a computed-jump target in the corpus (`dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_SetY:
	move.w	(a2)+,$42(a6)
	clr.w	$44(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetX - a2 op: sets the object's world x ($3E(a6)) from a word, fraction cleared
; In:  a2 = script: word x
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetX:
	move.w	(a2)+,$3E(a6)
	clr.w	$40(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_MovePos - a2 op: moves the object by (dx, dy) whole pixels
; In:  a2 = script: words dx, dy
; Out: NEXT
; Evidence: add.w to $3E/$42(a6); executed in the TAS
; ---------------------------------------------------------------------------
Op2_MovePos:
	move.w	(a2)+,d0
	add.w	d0,$3E(a6)
; ---------------------------------------------------------------------------
; Op2_MoveY - a2 op: moves the object vertically by dy whole pixels ($42(a6)): Op2_MovePos past its x
; In:  a2 = script: word dy
; Out: NEXT; d0.w = dy
; Evidence: add.w d0,$42(a6); a computed-jump target in the corpus (`dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_MoveY:
	move.w	(a2)+,d0
	add.w	d0,$42(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_MoveX - a2 op: moves the object horizontally by dx whole pixels
; In:  a2 = script: word dx
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_MoveX:
	move.w	(a2)+,d0
	add.w	d0,$3E(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_MoveXFacing - a2 op: moves the object horizontally by dx in its facing direction (dx negated when the sprite is H-flipped, bit 3 of byte $38(a5))
; In:  a2 = script: word dx
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_MoveXFacing:
	move.w	(a2)+,d0
	btst	#$3,$38(a5)
	beq.s	loc_001660
	neg.w	d0
loc_001660:
	add.w	d0,$3E(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_PlaceOnScreen - a2 op: places the object at screen point (x, y) (world = g_CameraX/g_CameraY + operand, fractions 0) and stops it (velocity $18/$1C and target speeds $24/$2C cleared)
; In:  a2 = script: words screen x, screen y
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_PlaceOnScreen:
	clr.l	$24(a3)
	clr.l	$18(a3)
	clr.l	$2C(a3)
	clr.l	$1C(a3)
; ---------------------------------------------------------------------------
; Op2_PlaceOnScreenKeepVel - a2 op: places the object at screen point (x, y) (world = g_CameraX/g_CameraY + operand, fractions 0) without stopping it: Op2_PlaceOnScreen past its four clr.l
; In:  a2 = script: words x, y
; Out: NEXT; d0/d1 = the new x/y longs
; Evidence: movem.l d0-d1,$3E(a6); a computed-jump target in the corpus (`dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_PlaceOnScreenKeepVel:
	moveq	#$0,d0
	move.w	(g_CameraX).w,d0
	add.w	(a2)+,d0
	swap	d0
	moveq	#$0,d1
	move.w	(g_CameraY).w,d1
	add.w	(a2)+,d1
	swap	d1
	movem.l	d0-d1,$3E(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_PlaceXOnScreen - a2 op: sets the object's x to g_CameraX + word (fraction 0) and clears its x velocity and x target speed
; In:  a2 = script: word screen x
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_PlaceXOnScreen:
	clr.l	$24(a3)
	clr.l	$18(a3)
	moveq	#$0,d0
	move.w	(g_CameraX).w,d0
	add.w	(a2)+,d0
	swap	d0
	move.l	d0,$3E(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_PlaceYOnScreen - a2 op: sets the object's y to g_CameraY + word (fraction 0) and clears its y velocity and y target speed
; In:  a2 = script: word screen y
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_PlaceYOnScreen:
	clr.l	$2C(a3)
	clr.l	$1C(a3)
; ---------------------------------------------------------------------------
; Op2_PlaceYOnScreenKeepVel - a2 op: sets the object's y to g_CameraY + word (fraction 0) without clearing its y velocity or target speed: Op2_PlaceYOnScreen past its two clr.l
; In:  a2 = script: word y
; Out: NEXT; d0 = the new y long
; Evidence: move.l d0,$42(a6); a computed-jump target in the corpus (`dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_PlaceYOnScreenKeepVel:
	moveq	#$0,d0
	move.w	(g_CameraY).w,d0
	add.w	(a2)+,d0
	swap	d0
	move.l	d0,$42(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTargetArgs - a2 op: sets the motion target $14/$16(a3) to (g_VM2Arg0, g_VM2Arg1) without re-aiming
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTargetArgs:
	move.w	(g_VM2Arg0).w,$14(a3)
	move.w	(g_VM2Arg1).w,$16(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTarget - a2 op: sets the motion target $14/$16(a3) to the world point (x, y) words without re-aiming
; In:  a2 = script: words x, y
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTarget:
	move.w	(a2)+,$14(a3)
; ---------------------------------------------------------------------------
; Op2_SetTargetY - a2 op: sets only the motion target's y $16(a3) from a word: Op2_SetTarget past its x
; In:  a2 = script: word y
; Out: NEXT
; Evidence: move.w (a2)+,$16(a3); a computed-jump target in the corpus (`dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_SetTargetY:
	move.w	(a2)+,$16(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTargetX - a2 op: sets only the motion target's x $14(a3) from a word
; In:  a2 = script: word x
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTargetX:
	move.w	(a2)+,$14(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
