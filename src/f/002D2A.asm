; ---------------------------------------------------------------------------
; Op2_Return - a2 op: returns from a script subroutine or a coroutine step: pops a2 pushed by Op2_Gosub* or Op2_StepCoroutineN and continues there
; In:  (sp) = return a2
; Out: a2 = popped; NEXT
; Evidence: movea.l (a7)+,a2; Scr2_002D28 (the idle coroutine) is just this op; all 153 runs
; ---------------------------------------------------------------------------
Op2_Return:
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_CallNativeRelSwitch - a2 op: calls native code at a relative address, then branches through a table of relative offsets indexed by the d0 it returns
; In:  a2 = script: word routine offset, then one word offset per possible d0 (0, 1, ...)
; Out: a2 = table[d0] target; NEXT
; Evidence: jsr (a0) / add.w d0,d0 / adda.w d0,a2 / adda.w (a2),a2; 125 runs; $1E7A4 `2D30 01FE 0004 001C` calls $1E9A4 (d0 = 1 while a countdown $30(a6) runs)
; ---------------------------------------------------------------------------
Op2_CallNativeRelSwitch:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	jsr	(a0)
	add.w	d0,d0
	adda.w	d0,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_CallNativeSwitch - a2 op: calls a native routine at an absolute address, then branches through a table of relative offsets indexed by the returned d0
; In:  a2 = script: long routine, then one word offset per d0 value
; Out: a2 = table[d0] target; NEXT
; Evidence: movea.l (a2)+,a0 / jsr (a0) / add.w d0,d0 / adda.w d0,a2 / adda.w (a2),a2; 151 runs; $1E94C `2D40 000093A0 FFE8 0002` (Camera_IsLocked: d0 = 0 or 1)
; ---------------------------------------------------------------------------
Op2_CallNativeSwitch:
	movea.l	(a2)+,a0
	jsr	(a0)
	add.w	d0,d0
	adda.w	d0,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ClearPlayerTarget - a2 op: drops the object's targeted player ($10(a4) = 0)
; In:  a2 = script (no operand); a4 = object's a4 block
; Out: NEXT
; Evidence: clr.w $10(a4); the other ops read $10(a4) as g_Player1/g_Player2 ($F650/$F690); 8 runs
; ---------------------------------------------------------------------------
Op2_ClearPlayerTarget:
	clr.w	$10(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BalancePlayerTarget - a2 op: makes sure the object targets a player ($10(a4)), spreading enemies between the two: keeps a targetable target unless the other player is targetable and was targeted by fewer objects last frame (record +$36), then switches; with no valid target picks one ($2DA2: the targetable player with fewer attackers, ties alternate by g_TargetTieFlip); counts itself in the chosen record's +$36/+$38
; In:  a2 = script: word offset; player records +$3A targetable byte, +$36 attackers last frame, +$38 attackers this frame (set by the player handler $102D6 each frame)
; Out: branch via the offset when a player is targeted, else (no targetable player) $10(a4) = 0 and NEXT after the offset
; Evidence: cmpa.l #$F650 picks the other record, compares $36, addq.w #1 $36/$38; $102D6 sets +$3A from var 1 bit 6 and copies +$38 to +$36 each frame; 122 runs; $27D54 `2D56 0002 2D22 ...`
; ---------------------------------------------------------------------------
Op2_BalancePlayerTarget:
	move.w	$10(a4),d0
	beq.s	loc_002DA2
	movea.w	d0,a0
	tst.b	$3A(a0)
	beq.s	loc_002DA2
	cmpa.l	#$FFFFF650,a0
	beq.s	loc_002D72
	lea.l	(g_Player1).w,a1
	bra.s	loc_002D76
loc_002D72:
	lea.l	(g_Player2).w,a1
loc_002D76:
	tst.b	$3A(a1)
	beq.s	loc_002D98
	move.w	$36(a0),d0
	cmp.w	$36(a1),d0
	ble.s	loc_002D98
	move.w	a1,$10(a4)
	addq.w	#$1,$36(a1)
	addq.w	#$1,$38(a1)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_002D98:
	addq.w	#$1,$38(a0)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_002DA2:
	lea.l	(g_Player1).w,a0
	tst.b	$3A(a0)
	bne.s	loc_002DBC
	tst.b	$7A(a0)
	bne.s	loc_002DD4
	clr.w	$10(a4)
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_002DBC:
	tst.b	$7A(a0)
	beq.s	loc_002DD8
	move.w	$36(a0),d0
	cmp.w	$76(a0),d0
	blt.s	loc_002DD8
	bgt.s	loc_002DD4
	not.b	(g_TargetTieFlip).w
	beq.s	loc_002DD8
loc_002DD4:
	lea.l	(g_Player2).w,a0
loc_002DD8:
	move.w	a0,$10(a4)
	addq.w	#$1,$36(a0)
	addq.w	#$1,$38(a0)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SwapPlayerTarget - a2 op: retargets the other player when that one is targetable; otherwise keeps a still-targetable current target; with no target at all picks one as Op2_BalancePlayerTarget does
; In:  a2 = script: word offset
; Out: branch via the offset when a player is targeted (counted in its +$38), else $10(a4) = 0 and NEXT after the offset
; Evidence: a1 = the record that is not $10(a4) ($F650 vs $F690); tst.b $3A(a1) -> move.w a1,$10(a4); 45 runs
; ---------------------------------------------------------------------------
Op2_SwapPlayerTarget:
	move.w	$10(a4),d0
	beq.s	loc_002DA2
	movea.w	d0,a0
	lea.l	(g_Player2).w,a1
	cmpi.w	#$F650,d0
	beq.s	loc_002E00
	lea.l	(g_Player1).w,a1
loc_002E00:
	tst.b	$3A(a1)
	bne.s	loc_002E20
	tst.b	$3A(a0)
	beq.s	loc_002E16
	addq.w	#$1,$38(a0)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_002E16:
	clr.w	$10(a4)
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_002E20:
	move.w	a1,$10(a4)
	addq.w	#$1,$38(a1)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_KeepPlayerTarget - a2 op: confirms the current player target for this frame: counts the object in the record's attackers (+$38) while it is targetable, drops the target ($10(a4) = 0) when it is not
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: tst.b $3A(a0) / addq.w #1,$38(a0) or clr.w $10(a4); 85 runs
; ---------------------------------------------------------------------------
Op2_KeepPlayerTarget:
	move.w	$10(a4),d0
	beq.w	VM2_Next
	movea.w	d0,a0
	tst.b	$3A(a0)
	beq.s	loc_002E46
	addq.w	#$1,$38(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_002E46:
	clr.w	$10(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchNoPlayerTarget - a2 op: branches when the object has no targetable player target (dropping a stale one); otherwise counts itself in the target's +$38 and continues
; In:  a2 = script: word offset
; Out: branch via Op2_Goto when none, else NEXT after the offset
; Evidence: beq Op2_Goto on $10(a4) = 0; tst.b $3A / clr.w $10(a4) / adda.w; 123 runs
; ---------------------------------------------------------------------------
Op2_BranchNoPlayerTarget:
	move.w	$10(a4),d0
	beq.w	Op2_Goto
	movea.w	d0,a0
	tst.b	$3A(a0)
	beq.s	loc_002E68
	addq.w	#$1,$38(a0)
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_002E68:
	clr.w	$10(a4)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_KeepOrPickPlayerTarget - a2 op: keeps a still-targetable player target (counted, no branch); otherwise picks a new one ($2DA2) and branches when it found one
; In:  a2 = script: word offset
; Out: kept: NEXT after the offset; new target: branch; none: $10(a4) = 0, NEXT after the offset
; Evidence: beq/beq.w $2DA2 (the picker of Op2_BalancePlayerTarget) else addq.w #1,$38(a0) / addq.l #2,a2; 57 runs
; ---------------------------------------------------------------------------
Op2_KeepOrPickPlayerTarget:
	move.w	$10(a4),d0
	beq.w	loc_002DA2
	movea.w	d0,a0
	tst.b	$3A(a0)
	beq.w	loc_002DA2
	addq.w	#$1,$38(a0)
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchPlayerRight - a2 op: branches when the targeted player's x (record +$2E) is at or right of the object's x ($3E(a6)); no target skips
; In:  a2 = script: word offset
; Out: branch via the offset, else NEXT after it (Op2_SkipWord)
; Evidence: cmp.w $3E(a6) with $2E(a0) / blt Op2_SkipWord; the player handler $102D6 copies its x/y to record +$2E/+$32; 11 runs
; ---------------------------------------------------------------------------
Op2_BranchPlayerRight:
	move.w	$10(a4),d0
	beq.w	Op2_SkipWord
	movea.w	d0,a0
	move.w	$2E(a0),d0
	cmp.w	$3E(a6),d0
	blt.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchPlayerLeft - a2 op: branches when the targeted player's x (record +$2E) is at or left of the object's x; no target skips
; In:  a2 = script: word offset
; Out: branch via the offset, else NEXT after it
; Evidence: cmp.w $3E(a6) / bgt Op2_SkipWord; 8 runs
; ---------------------------------------------------------------------------
Op2_BranchPlayerLeft:
	move.w	$10(a4),d0
	beq.w	Op2_SkipWord
	movea.w	d0,a0
	move.w	$2E(a0),d0
	cmp.w	$3E(a6),d0
	bgt.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchPlayerBelow - a2 op: branches when the targeted player's y (record +$32) is at or below the object's y ($42(a6)); no target skips
; In:  a2 = script: word offset
; Out: branch via the offset, else NEXT after it
; Evidence: cmp.w $42(a6) with $32(a0) / blt Op2_SkipWord; 7 runs (not the TAS)
; ---------------------------------------------------------------------------
Op2_BranchPlayerBelow:
	move.w	$10(a4),d0
	beq.w	Op2_SkipWord
	movea.w	d0,a0
	move.w	$32(a0),d0
	cmp.w	$42(a6),d0
	blt.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchPlayerAbove - a2 op: branches when the targeted player's y (record +$32) is at or above the object's y; no target skips
; In:  a2 = script: word offset
; Out: branch via the offset, else NEXT after it
; Evidence: cmp.w $42(a6) / bgt Op2_SkipWord; 7 runs
; ---------------------------------------------------------------------------
Op2_BranchPlayerAbove:
	move.w	$10(a4),d0
	beq.w	Op2_SkipWord
	movea.w	d0,a0
	move.w	$32(a0),d0
	cmp.w	$42(a6),d0
	bgt.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FaceCruiseToPlayerX - a2 op: turns toward the targeted player and cruises toward its x: x target speed $24(a3) = +-cruise $30(a3), H flip bit 3 of $38(a5) cleared (right) or set (left); no target brakes x
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: move.l $30(a3),$24(a3) / bclr #3,$38(a5), or neg + bset; no target -> Op2_ClearTargetSpeedX; 32 runs
; ---------------------------------------------------------------------------
Op2_FaceCruiseToPlayerX:
	move.w	$10(a4),d0
	beq.w	Op2_ClearTargetSpeedX
	movea.w	d0,a0
	move.w	$2E(a0),d0
	cmp.w	$3E(a6),d0
	blt.s	loc_002F22
	move.l	$30(a3),$24(a3)
	bclr	#$3,$38(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_002F22:
	move.l	$30(a3),d0
	neg.l	d0
	move.l	d0,$24(a3)
	bset	#$3,$38(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
