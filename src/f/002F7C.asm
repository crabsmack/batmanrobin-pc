; ---------------------------------------------------------------------------
; Op2_CruiseToPlayerX - a2 op: cruises toward the targeted player's x without changing the facing (x target speed +cruise, or Op2_CruiseLeft); no target brakes x
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: blt Op2_CruiseLeft / move.l $30(a3),$24(a3); 43 runs
; ---------------------------------------------------------------------------
Op2_CruiseToPlayerX:
	move.w	$10(a4),d0
	beq.w	Op2_ClearTargetSpeedX
	movea.w	d0,a0
	move.w	$2E(a0),d0
	cmp.w	$3E(a6),d0
	blt.w	Op2_CruiseLeft
	move.l	$30(a3),$24(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
