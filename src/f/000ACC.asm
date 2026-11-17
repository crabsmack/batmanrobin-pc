; ---------------------------------------------------------------------------
; Op2_RunSpawnAction - a2 op: when the object has a spawn action ($19(a6) nonzero), clears it and runs entry $19 of Tbl_012EF0 (SpawnAction_Run) at the object's position with d1 = its argument $1A(a6); continues (NEXT)
; In:  a2 = script (no operands); a6 = object
; Out: NEXT; the action may spawn objects (table entries $1393C... set up scripts at the object's x/y)
; Evidence: also the tail of Op2_AwardScoreToHitter ($A48) and Op2_AwardScoreToPlayers ($A80); Tbl_012EF0 entry 0 is an rts, 1/2 are the group counters $6F70/$6F9A; the file also holds Op2_SetSpeed/Rand ($AE6/$AF2), the sprite attribute ops $B0E-$B4A; 1.7k TAS frames
; ---------------------------------------------------------------------------
Op2_RunSpawnAction:
	move.b	$19(a6),d0
	beq.w	VM2_Next
	clr.b	$19(a6)
	move.w	$1A(a6),d1
	jsr	(SpawnAction_Run).l
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetSpeedRand - a2 op: sets the speed factor to base + random(range) (Rand_Below), then as Op2_SetSpeed
; In:  a2 = script: words range, base
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetSpeedRand:
	move.w	(a2)+,d0
	jsr	(Rand_Below).l
	add.w	(a2)+,d0
	bra.s	loc_000AF4
; ---------------------------------------------------------------------------
; Op2_SetSpeed - a2 op: sets the object's speed factor $2E(a4) = word * g_TimeScale >> 8, its timer tick $30(a4) = $2E >> 4 and $32(a4) = tick squared
; In:  a2 = script: word speed ($100 = 1.0)
; Out: NEXT
; Evidence: $30(a4) is what every timer op subtracts per run ($E56-$11B4); $2E(a4) scales the turn rate (op $12BA); executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetSpeed:
	move.w	(a2)+,d0
loc_000AF4:
	mulu.w	(g_TimeScale).w,d0
	lsr.l	#$8,d0
	move.w	d0,$2E(a4)
	lsr.w	#$4,d0
	move.w	d0,$30(a4)
	mulu.w	d0,d0
	move.w	d0,$32(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetSprAttr - a2 op: sets the object's sprite attribute word $38(a5) (d6 for Spr_Add*: priority, palette, flip; bit 11 = H flip)
; In:  a2 = script: word attributes
; Out: NEXT
; Evidence: $4524 passes $38(a5) as d6 to Spr_AddSorted; other ops test bit 3 of byte $38(a5) for facing; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetSprAttr:
	move.w	(a2)+,$38(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ModSprAttr - a2 op: and/eor on the high byte of the sprite attributes $38(a5) (sets, clears or toggles flip/palette bits)
; In:  a2 = script: byte and-mask, byte eor-mask
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_ModSprAttr:
	move.b	(a2)+,d0
	and.b	d0,$38(a5)
	move.b	(a2)+,d0
	eor.b	d0,$38(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchSprAttrEq - a2 op: branches when (high byte of $38(a5) & mask) == value (beq.w Op2_Goto; the name was Ne before 2026-10-28, swapped with its sibling to match the code)
; In:  a2 = script: byte mask, byte value, word offset
; Out: branch, else NEXT after the offset
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_BranchSprAttrEq:
	move.b	$38(a5),d0
	and.b	(a2)+,d0
	cmp.b	(a2)+,d0
	beq.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchSprAttrNe - a2 op: branches when (high byte of $38(a5) & mask) != value (bne.w Op2_Goto; the name was Eq before 2026-10-28)
; In:  a2 = script: byte mask, byte value, word offset
; Out: branch, else NEXT after the offset
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_BranchSprAttrNe:
	move.b	$38(a5),d0
	and.b	(a2)+,d0
	cmp.b	(a2)+,d0
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetSprDepth - a2 op: sets the sprite sort depth $3A(a5) (d7 for Spr_AddSorted, higher draws in front)
; In:  a2 = script: word depth
; Out: NEXT
; Evidence: $4524/$4570 pass $3A(a5) as d7; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetSprDepth:
	move.w	(a2)+,$3A(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
