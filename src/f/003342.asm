; ---------------------------------------------------------------------------
; Op2_FaceCruiseToPlayerXTurnBranch - a2 op: as Op2_FaceCruiseToPlayerX (face and cruise toward the targeted player's x) and branches when that turned the object around
; In:  a2 = script: word offset
; Out: branch when the H flip bit changed, else NEXT after the offset; no target skips with the speed unchanged
; Evidence: bclr/bset #3,$38(a5) then beq/bne Op2_SkipWord; 19 runs
; ---------------------------------------------------------------------------
Op2_FaceCruiseToPlayerXTurnBranch:
	move.w	$10(a4),d0
	beq.w	Op2_SkipWord
	movea.w	d0,a0
	move.w	$2E(a0),d0
	cmp.w	$3E(a6),d0
	blt.s	loc_00336C
	move.l	$30(a3),$24(a3)
	bclr	#$3,$38(a5)
	beq.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_00336C:
	move.l	$30(a3),d0
	neg.l	d0
	move.l	d0,$24(a3)
	bset	#$3,$38(a5)
	bne.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FacePlayer - a2 op: turns the object toward the targeted player (H flip bit 3 of $38(a5) set when the player is to the left), no movement change
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: cmp.w $2E(a0) with $3E(a6) / bclr or bset #3,$38(a5); 25 runs
; ---------------------------------------------------------------------------
Op2_FacePlayer:
	move.w	$10(a4),d0
	beq.w	VM2_Next
	movea.w	d0,a0
	move.w	$2E(a0),d0
	cmp.w	$3E(a6),d0
	blt.s	loc_0033A4
	bclr	#$3,$38(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_0033A4:
	bset	#$3,$38(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FacePlayerTurnBranch - a2 op: turns toward the targeted player and branches when that changed the facing
; In:  a2 = script: word offset
; Out: branch when it turned, else NEXT after the offset (also with no target)
; Evidence: bclr/bset #3,$38(a5) then beq/bne Op2_SkipWord; 19 runs
; ---------------------------------------------------------------------------
Op2_FacePlayerTurnBranch:
	move.w	$10(a4),d0
	beq.w	Op2_SkipWord
	movea.w	d0,a0
	move.w	$2E(a0),d0
	cmp.w	$3E(a6),d0
	blt.s	loc_0033D2
	bclr	#$3,$38(a5)
	beq.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_0033D2:
	bset	#$3,$38(a5)
	bne.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_CruiseToPlayerY - a2 op: cruises toward the targeted player's y (y target speed $2C(a3) = +cruise $34(a3), or Op2_CruiseUp); no target brakes y
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: cmp.w $32(a0) with $42(a6) / blt Op2_CruiseUp / move.l $34(a3),$2C(a3); 54 runs
; ---------------------------------------------------------------------------
Op2_CruiseToPlayerY:
	move.w	$10(a4),d0
	beq.w	Op2_ClearTargetSpeedY
	movea.w	d0,a0
	move.w	$32(a0),d0
	cmp.w	$42(a6),d0
	blt.w	Op2_CruiseUp
	move.l	$34(a3),$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
