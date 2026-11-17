; ---------------------------------------------------------------------------
; Op2_StepVelocity - a2 op: steps the motion block's velocity for this frame and leaves the frame's move in d1/d2 for the next op: each axis moves toward its target speed by its acceleration (copied to $38/$3C), then the decaying impulses $2/$6 are added; while the stun count $1(a3) runs the velocity part is 0
; In:  a2 = script (no operand); a3 = motion block (+$18/$1C velocity, +$20/$28 accel, +$24/$2C target speed, +$2/$6 impulse, +$1 stun count)
; Out: d1/d2 = x/y move (16.16) for Op2_MoveTerrain/Op2_ApplyMove/the clamps; NEXT
; Evidence: movem.l $18(a3),d1-d6 / movem.l d1-d2,$18(a3) and $38(a3); impulses decay by $1400 a run; PlayerAtk_HitPause sets $1(a3) = 6; every object loop (125k TAS frames, all 160 runs), e.g. $11B84 `4B74 4C52`, $133A8 `4B74 4CDA`
; ---------------------------------------------------------------------------
Op2_StepVelocity:
	tst.b	$1(a3)
	beq.s	loc_004B84
	subq.b	#$1,$1(a3)
	moveq	#$0,d1
	moveq	#$0,d2
	bra.s	Op2_StepVelocity_Impulse
loc_004B84:
	movem.l	$18(a3),d1-d6
; ---------------------------------------------------------------------------
; Op2_StepVelocity_Steer - tail of Op2_StepVelocity (also entered by Mot_SteerToHeading): moves d1 toward d4 by d3 and d2 toward d6 by d5, stores them as the velocity, then adds the impulses
; In:  d1/d2 velocity, d3/d5 acceleration, d4/d6 target speed; a3 = motion block
; Out: as Op2_StepVelocity
; Evidence: bra.w from $156A (three sites); cmp.l d4,d1 / sub.l d3,d1 / add.l d3,d1 clamps
; ---------------------------------------------------------------------------
Op2_StepVelocity_Steer:
	cmp.l	d4,d1
	blt.s	loc_004B96
	sub.l	d3,d1
	cmp.l	d4,d1
	bgt.s	loc_004B9E
	bra.s	loc_004B9C
loc_004B96:
	add.l	d3,d1
	cmp.l	d4,d1
	blt.s	loc_004B9E
loc_004B9C:
	move.l	d4,d1
loc_004B9E:
	cmp.l	d6,d2
	blt.s	loc_004BAA
	sub.l	d5,d2
	cmp.l	d6,d2
	bgt.s	loc_004BB2
	bra.s	loc_004BB0
loc_004BAA:
	add.l	d5,d2
	cmp.l	d6,d2
	blt.s	loc_004BB2
loc_004BB0:
	move.l	d6,d2
loc_004BB2:
	movem.l	d1-d2,$18(a3)
	movem.l	d1-d2,$38(a3)
; ---------------------------------------------------------------------------
; Op2_StepVelocity_Impulse - tail of Op2_StepVelocity (also entered by Mot_SteerToHeading while stunned): decays the impulses $2/$6(a3) by $1400 toward 0 and adds them to the move d1/d2
; In:  d1/d2 = move so far; a3 = motion block
; Out: d1/d2 += impulses; NEXT
; Evidence: subi.l/addi.l #$1400 on $2(a3) and $6(a3); bra.w from $156A
; ---------------------------------------------------------------------------
Op2_StepVelocity_Impulse:
	move.l	$2(a3),d0
	beq.s	loc_004BE0
	bpl.s	loc_004BD0
	addi.l	#$1400,d0
	bmi.s	loc_004BDA
	bra.s	loc_004BD8
loc_004BD0:
	subi.l	#$1400,d0
	bpl.s	loc_004BDA
loc_004BD8:
	moveq	#$0,d0
loc_004BDA:
	move.l	d0,$2(a3)
	add.l	d0,d1
loc_004BE0:
	move.l	$6(a3),d0
	beq.w	VM2_Next
	bpl.s	loc_004BF4
	addi.l	#$1400,d0
	bmi.s	loc_004BFE
	bra.s	loc_004BFC
loc_004BF4:
	subi.l	#$1400,d0
	bpl.s	loc_004BFE
loc_004BFC:
	moveq	#$0,d0
loc_004BFE:
	move.l	d0,$6(a3)
	add.l	d0,d2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ClampMoveToArea - a2 op: trims this frame's move d1/d2 so the object stays inside the movement area (g_MoveAreaLeft/Top, g_MoveAreaW/H)
; In:  a2 = script (no operand); d1/d2 = move from Op2_StepVelocity
; Out: d1/d2 clamped; NEXT
; Evidence: y: $42(a6)+d2-g_MoveAreaTop, minus g_MoveAreaH; x: the same with Left/W ($4C1E); 71 corpus runs; $4CF84 `4C08 4C52`
; ---------------------------------------------------------------------------
Op2_ClampMoveToArea:
	move.l	$42(a6),d0
	add.l	d2,d0
	sub.l	(g_MoveAreaTop).w,d0
	bmi.s	loc_004C1C
	sub.l	(g_MoveAreaH).w,d0
	bmi.w	Op2_ClampMoveToAreaX
loc_004C1C:
	sub.l	d0,d2
; ---------------------------------------------------------------------------
; Op2_ClampMoveToAreaX - a2 op: trims only the x move d1 to keep the object between g_MoveAreaLeft and g_MoveAreaLeft + g_MoveAreaW
; In:  a2 = script (no operand); d1 = x move
; Out: d1 clamped; NEXT
; Evidence: second half of Op2_ClampMoveToArea, also dispatched alone (89 corpus runs); $3C734 `4C1E 4CDA`
; ---------------------------------------------------------------------------
Op2_ClampMoveToAreaX:
	move.l	$3E(a6),d0
	add.l	d1,d0
	sub.l	(g_MoveAreaLeft).w,d0
	bmi.s	loc_004C32
	sub.l	(g_MoveAreaW).w,d0
	bmi.w	VM2_Next
loc_004C32:
	sub.l	d0,d1
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ClampMoveToAreaY - a2 op: trims only the y move d2 to keep the object between g_MoveAreaTop and g_MoveAreaTop + g_MoveAreaH
; In:  a2 = script (no operand); d2 = y move
; Out: d2 clamped; NEXT
; Evidence: sub.l g_MoveAreaTop / g_MoveAreaH / sub.l d0,d2; 8 corpus runs; $424AA `4C38 4C52`
; ---------------------------------------------------------------------------
Op2_ClampMoveToAreaY:
	move.l	$42(a6),d0
	add.l	d2,d0
	sub.l	(g_MoveAreaTop).w,d0
	bmi.s	loc_004C4C
	sub.l	(g_MoveAreaH).w,d0
	bmi.w	VM2_Next
loc_004C4C:
	sub.l	d0,d2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ApplyMove - a2 op: adds this frame's move d1/d2 to the object's x/y with no terrain test
; In:  a2 = script (no operand); d1/d2 = move (16.16)
; Out: $3E/$42(a6) moved; NEXT
; Evidence: add.l d1,$3E(a6) / add.l d2,$42(a6); 143 corpus runs; $11B84 `4B74 4C52`
; ---------------------------------------------------------------------------
Op2_ApplyMove:
	add.l	d1,$3E(a6)
	add.l	d2,$42(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
