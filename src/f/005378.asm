; ---------------------------------------------------------------------------
; Op2_FxMarkPlayerTarget - a2 op: stores the targeted player's x/y (record +$2E/+$32 of $10(a4)) in g_ArcTargetX/Y for the effect script spawned next; with no target skips the next 4 script bytes
; In:  a2 = script (no operand); $10(a4) = targeted player record (0 = none)
; Out: NEXT; no target: NEXT after skipping 4 bytes (the following Op2_FxSpawnRegs op word and its offset)
; Evidence: followed by $5198 Op2_FxSpawnRegs + word in its script uses ($1B446, $1F340, $298C2, $42C98); the spawned a4 scripts call Arc_PlanToMark, which reads g_ArcTargetX/Y; 16 corpus runs, 23 TAS frames
; ---------------------------------------------------------------------------
Op2_FxMarkPlayerTarget:
	move.w	$10(a4),d0
	beq.s	loc_005390
	movea.w	d0,a0
	move.w	$2E(a0),(g_ArcTargetX).w
	move.w	$32(a0),(g_ArcTargetY).w
	movea.w	(a2)+,a0
	jmp	(a0)
loc_005390:
	addq.l	#$4,a2
	movea.w	(a2)+,a0
	jmp	(a0)
