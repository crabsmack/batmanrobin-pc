; ---------------------------------------------------------------------------
; Op2_SavePos - a2 op: saves the object's position (x $3E, y $42, 16.16 longs) in $46/$4A(a6)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: the file also holds Op2_SwapSavedPos, Op2_StepSavedPos, Op2_SetTurnRate, Op2_SetHeading; 3 TAS frames
; ---------------------------------------------------------------------------
Op2_SavePos:
	move.l	$42(a6),$4A(a6)
; ---------------------------------------------------------------------------
; Op2_SaveX - a2 op: saves only the object's x ($3E(a6), 16.16 long) in $46(a6): Op2_SavePos past its y copy
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: move.l $3E(a6),$46(a6); a computed-jump target in the corpus (`dkit genesis recomp entries`)
; ---------------------------------------------------------------------------
Op2_SaveX:
	move.l	$3E(a6),$46(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SwapSavedPos - a2 op: exchanges the object's position $3E/$42(a6) with the saved position $46/$4A(a6)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: movem.l $3E(a6),d0-d3 / exg / movem.l back; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SwapSavedPos:
	movem.l	$3E(a6),d0-d3
	exg.l	d0,d2
	exg.l	d1,d3
	movem.l	d0-d3,$3E(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_StepSavedPos - a2 op: adds the motion block's velocity ($18/$1C(a3)) to the saved position $46/$4A(a6)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_StepSavedPos:
	move.l	$18(a3),d0
	add.l	d0,$46(a6)
	move.l	$1C(a3),d0
	add.l	d0,$4A(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTurnRate - a2 op: sets the heading turn rate $10(a3) (long, 16.16 heading units per run) to word * speed factor $2E(a4)
; In:  a2 = script: word rate
; Out: NEXT
; Evidence: Mot_TurnTowardHeading steps $C(a3) by $10(a3); executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetTurnRate:
	move.w	(a2)+,d0
	muls.w	$2E(a4),d0
	move.l	d0,$10(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetHeading - a2 op: sets the target heading $A(a3) to word & $3FF (0 = +x, $100 = +y, 1024 per turn)
; In:  a2 = script: word heading
; Out: NEXT
; Evidence: Mot_Atan2 (atan2) returns 0 for +x and $100/$300 for +y/-y; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetHeading:
	move.w	(a2)+,d0
	andi.w	#$3FF,d0
	move.w	d0,$A(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
