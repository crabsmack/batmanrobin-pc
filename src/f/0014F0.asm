; ---------------------------------------------------------------------------
; Op2_LaunchAlongHeading - a2 op: sets the velocity to the cruise speeds $30/$34(a3) along the target heading $A(a3) ($18 = speed x * cos, $1C = speed y * sin, Tbl_059B22) and copies the cruise speeds into the target speeds $24/$2C
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: Tbl_059B22 is indexed by heading & $3FE with cos at +$100 (a quarter turn); the file also holds Op2_AimAtTracked and the turn ops $1554-$1562; 332 TAS frames
; ---------------------------------------------------------------------------
Op2_LaunchAlongHeading:
	lea.l	(Tbl_059B22).l,a0
	move.w	$A(a3),d0
	andi.w	#$3FE,d0
	adda.w	d0,a0
	move.l	$30(a3),d1
	move.l	$34(a3),d2
	move.l	d1,$24(a3)
	move.l	d2,$2C(a3)
	asr.l	#$8,d1
	asr.l	#$8,d2
	muls.w	$100(a0),d1
	muls.w	(a0),d2
	asr.l	#$7,d1
	asr.l	#$7,d2
	movem.l	d1-d2,$18(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AimAtTracked - a2 op: points the target heading at the position held in the block whose handle is $10(a4) (x at +$2E, y at +$32); does nothing when $10(a4) is 0
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: $102D6 keeps such a block ($1E(a6)) updated with an object's $3E/$42; executed in the TAS
; ---------------------------------------------------------------------------
Op2_AimAtTracked:
	move.w	$10(a4),d0
	beq.w	VM2_Next
	movea.w	d0,a0
	move.w	$2E(a0),d0
	sub.w	$3E(a6),d0
	move.w	$32(a0),d1
	sub.w	$42(a6),d1
	jsr	(Mot_Atan2).l
	andi.w	#$3FF,d2
	move.w	d2,$A(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SnapHeadingSteer - a2 op: sets the current heading to the target heading at once, then steers the velocity along it (Mot_SteerToHeading)
; In:  a2 = script (no operand)
; Out: NEXT (from Mot_SteerToHeading)
; Evidence: move.w $A(a3),$C(a3) / bra $15BE; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SnapHeadingSteer:
	move.w	$A(a3),$C(a3)
	bra.s	Mot_SteerToHeading
; ---------------------------------------------------------------------------
; Op2_TurnSteer - a2 op: turns the current heading toward the target by the turn rate (Mot_TurnTowardHeading), then steers the velocity along it
; In:  a2 = script (no operand)
; Out: NEXT (from Mot_SteerToHeading)
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_TurnSteer:
	bsr.w	Mot_TurnTowardHeading
	bra.s	Mot_SteerToHeading
; ---------------------------------------------------------------------------
; Op2_TurnHeading - a2 op: turns the current heading toward the target heading by the turn rate, without touching the velocity
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_TurnHeading:
	bsr.w	Mot_TurnTowardHeading
	movea.w	(a2)+,a0
	jmp	(a0)
