; ---------------------------------------------------------------------------
; Obj_TestBoxStruck - tests and clears the struck bit of one body box chosen by a word operand (an index out of range or an empty slot counts as not struck)
; In:  a2 = word index; a4 = object
; Out: d0 = 1 and g_StruckBox = the box when struck, else d0 = 0; a2 past the index
; Evidence: cmp.w $2(a4) / move.w (a1,d0.w) / bclr #0,$3C(a0) / bne $3982; 7 runs
; ---------------------------------------------------------------------------
Obj_TestBoxStruck:
	move.w	(a2)+,d0
	cmp.w	$2(a4),d0
	bge.s	loc_0039A8
	lea.l	$4(a4),a1
	add.w	d0,d0
	move.w	(a1,d0.w),d0
	beq.s	loc_0039A8
	movea.w	d0,a0
	bclr	#$0,$3C(a0)
	bne.s	Obj_SetStruckBox
loc_0039A8:
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Op2_SetEnergy1P2P - a2 op: sets the object's energy from one of two operands, the second when two players are in play (g_PlayersInPlay = 2), then scales it as Op2_SetEnergy
; In:  a2 = script: word energy (1 player), word energy (2 players)
; Out: $52(a6) energy, $54(a6) = energy/100; NEXT
; Evidence: cmpi.w #2,g_PlayersInPlay picks the operand, then the shared scaling at $39BC; 42 runs; $1809E `39AC 2200 3200`
; ---------------------------------------------------------------------------
Op2_SetEnergy1P2P:
	move.w	(a2)+,d1
	cmpi.w	#$2,(g_PlayersInPlay).w
	beq.s	Op2_SetEnergy
	addq.l	#$2,a2
	bra.s	loc_0039BC
; ---------------------------------------------------------------------------
; Op2_SetEnergy - a2 op: sets the object's energy $52(a6) = operand * g_EnemyEnergyScale >> 8 (g_BossEnergyScale for owner class $B) and its 1% unit $54(a6) = energy / 100
; In:  a2 = script: word energy
; Out: $52/$54(a6) set; NEXT
; Evidence: mulu.w d1,d0 / lsr.l #8 / divu.w #$64; dispatched at $39BA in 125 runs (an entry inside Op2_SetEnergy1P2P's code); $1ACF4 `39BA 00D0`; the boss meter reads $52 (BossMeter_Show)
; ---------------------------------------------------------------------------
Op2_SetEnergy:
	move.w	(a2)+,d1
loc_0039BC:
	move.w	(g_EnemyEnergyScale).w,d0
	cmpi.w	#$B,$58(a6)
	bne.s	loc_0039CC
	move.w	(g_BossEnergyScale).w,d0
loc_0039CC:
	mulu.w	d1,d0
	lsr.l	#$8,d0
	move.w	d0,$52(a6)
	andi.l	#$FFFF,d0
	divu.w	#$64,d0
	move.w	d0,$54(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchEnergyAtPct - a2 op: branches when the object's energy $52(a6) is at or below the given percentage of the energy it was given (N * $54)
; In:  a2 = script: word percent N, word offset
; Out: branch via Op2_Goto, else NEXT after the offset
; Evidence: mulu.w (a2)+ with $54(a6) / cmp.w $52(a6) / bge Op2_Goto; 11 runs; $3E034 `39E6 000A 0004` (10%)
; ---------------------------------------------------------------------------
Op2_BranchEnergyAtPct:
	move.w	$54(a6),d0
	mulu.w	(a2)+,d0
	cmp.w	$52(a6),d0
	bge.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_TakeDamage - a2 op: subtracts the struck box's damage (g_StruckBox +$20, then cleared) from the object's energy; branches when the energy runs out (clamped to 0)
; In:  a2 = script: word offset; g_StruckBox from Op2_BranchStruck
; Out: energy > 0: NEXT after the offset (Op2_SkipWord); else $52(a6) = 0 and branch
; Evidence: sub.w d0,$52(a6) / bgt Op2_SkipWord / clr.w / adda.w; 97 runs
; ---------------------------------------------------------------------------
Op2_TakeDamage:
	movea.l	(g_StruckBox).w,a0
	move.w	$20(a0),d0
	clr.w	$20(a0)
	sub.w	d0,$52(a6)
	bgt.w	Op2_SkipWord
	clr.w	$52(a6)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_TakeKnockback - a2 op: applies the struck box's knockback: copies g_StruckBox +$22.l (the attack's push, signed by the attacker's facing) into the motion block's x impulse $2(a3)
; In:  a2 = script (no operand); g_StruckBox set
; Out: NEXT
; Evidence: move.l $22(a0),$2(a3); the hit callbacks store +$22 from their knockback argument ($107B0: $36 << 8, negated by the attacker's H flip); 20 runs
; ---------------------------------------------------------------------------
Op2_TakeKnockback:
	movea.l	(g_StruckBox).w,a0
	move.l	$22(a0),$2(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_TakeHit - a2 op: Op2_TakeKnockback then Op2_TakeDamage in one: copies the struck box's push to the x impulse, subtracts its damage and branches when the energy runs out
; In:  a2 = script: word offset; g_StruckBox set
; Out: energy > 0: NEXT after the offset; else $52(a6) = 0 and branch
; Evidence: move.l $22(a0),$2(a3) then the Op2_TakeDamage body; 80 runs
; ---------------------------------------------------------------------------
Op2_TakeHit:
	movea.l	(g_StruckBox).w,a0
	move.l	$22(a0),$2(a3)
	move.w	$20(a0),d0
	clr.w	$20(a0)
	sub.w	d0,$52(a6)
	bgt.w	Op2_SkipWord
	clr.w	$52(a6)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetBossEnergy - a2 op: shows the object's energy on the boss meter (g_BossEnergy = $52(a6))
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: move.w $52(a6),g_BossEnergy; 38 runs
; ---------------------------------------------------------------------------
Op2_SetBossEnergy:
	move.w	$52(a6),(g_BossEnergy).w
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AddBossEnergy - a2 op: adds the object's energy $52(a6) to g_BossEnergy (bosses made of several parts)
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: add.w d0,g_BossEnergy; 10 runs; BossMeter_AddPart adds parts to g_BossEnergyMax the same way
; ---------------------------------------------------------------------------
Op2_AddBossEnergy:
	move.w	$52(a6),d0
	add.w	d0,(g_BossEnergy).w
	movea.w	(a2)+,a0
	jmp	(a0)
