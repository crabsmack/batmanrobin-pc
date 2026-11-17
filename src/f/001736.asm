; ---------------------------------------------------------------------------
; Op2_TargetRandomScreen - a2 op: picks a random motion target near the screen centre: x = 160 + random within +-75, y = 112 + random within +-55 (screen pixels)
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT; $14/$16(a3) set
; Evidence: Rand_Signed (signed random * d0 >> 16) with $96/$6E, then + $A0/$70; the screen seek ops ($1922, $1A62) compare $14/$16 with x - g_CameraX / y - g_CameraY; TAS frames 116871-117395 (1 run)
; ---------------------------------------------------------------------------
Op2_TargetRandomScreen:
	move.w	#$96,d0
	jsr	(Rand_Signed).l
	addi.w	#$A0,d0
	move.w	d0,$14(a3)
	move.w	#$6E,d0
	jsr	(Rand_Signed).l
	addi.w	#$70,d0
	move.w	d0,$16(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_TargetRandomScreenArea - a2 op: picks a random motion target around the screen centre (160, 112) within the given x and y ranges
; In:  a2 = script: word x range, word y range (random within +-range/2 each); a3 = motion block
; Out: NEXT; $14/$16(a3) set
; Evidence: as Op2_TargetRandomScreen with the ranges from the operands; 3 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_TargetRandomScreenArea:
	move.w	(a2)+,d0
	jsr	(Rand_Signed).l
	addi.w	#$A0,d0
	move.w	d0,$14(a3)
	move.w	(a2)+,d0
	jsr	(Rand_Signed).l
	addi.w	#$70,d0
	move.w	d0,$16(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetAccelForDist - a2 op: sets both motion accelerations (x $20, y $28 of a3) to the value Mot_AccelForDistance computes from a distance and a time
; In:  a2 = script: word distance, word time (timer units); a3 = motion block, a4 = speed factor block
; Out: NEXT
; Evidence: bsr Mot_AccelForDistance, d1 to $20 and $28; the steering tail $4B8A moves velocity toward the target speeds by these steps; 94 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetAccelForDist:
	bsr.w	Mot_AccelForDistance
	move.l	d1,$20(a3)
	move.l	d1,$28(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetCruise - a2 op: sets both cruise speeds (x $30, y $34 of a3) to word * the object's speed factor $2E(a4)
; In:  a2 = script: word speed (signed); a3 = motion block
; Out: NEXT
; Evidence: muls.w $2E(a4); the cruise speeds feed the target speeds in Op2_CruiseRight/Left/Up/Down and Op2_LaunchAlongHeading; 85 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetCruise:
	move.w	(a2)+,d0
	muls.w	$2E(a4),d0
	move.l	d0,$30(a3)
	move.l	d0,$34(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ClearImpulseX - a2 op: cancels the motion block's decaying x impulse (long $2(a3)) that the steering tail adds to the x velocity
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: clr.l $2(a3); $4BBE adds $2(a3) to the x velocity and decays it by $1400 per run (y impulse at $6); seen in 1 fuzz run only (not the TAS)
; ---------------------------------------------------------------------------
Op2_ClearImpulseX:
	clr.l	$2(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetAccelXForDist - a2 op: sets the x acceleration $20(a3) from a distance and a time (Mot_AccelForDistance)
; In:  a2 = script: word distance, word time; a3 = motion block
; Out: NEXT
; Evidence: bsr Mot_AccelForDistance then move.l d1,$20(a3); 152 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetAccelXForDist:
	bsr.w	Mot_AccelForDistance
	move.l	d1,$20(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetCruiseX - a2 op: sets the x cruise speed $30(a3) to word * the speed factor $2E(a4)
; In:  a2 = script: word speed (signed); a3 = motion block
; Out: NEXT
; Evidence: muls.w $2E(a4) to $30(a3); 152 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetCruiseX:
	move.w	(a2)+,d0
	muls.w	$2E(a4),d0
	move.l	d0,$30(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetVelXForward - a2 op: sets the x velocity $18(a3) to word * speed factor in the facing direction (negated while the sprite is H-flipped, bit 3 of $38(a5))
; In:  a2 = script: word speed; a3 = motion block, a5 = sprite block
; Out: NEXT (the not-flipped path exits through $7E0)
; Evidence: mulu.w $2E(a4), neg.l when btst #3,$38(a5); 47 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetVelXForward:
	move.w	(a2)+,d0
	mulu.w	$2E(a4),d0
	move.l	d0,$18(a3)
	btst	#$3,$38(a5)
	beq.w	VM2_Next
	neg.l	$18(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
