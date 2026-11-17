; ---------------------------------------------------------------------------
; Op4_SpawnHeadingFromSelf - a4 op: sets the spawn heading register d3 to the object's heading $2A and the spawn radius d2 to 0
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: move.l $2A(a6),d3 / moveq #0,d2; script uses $42994, $46AFA; 43 corpus runs
; ---------------------------------------------------------------------------
Op4_SpawnHeadingFromSelf:
	move.l	$2A(a6),d3
	moveq	#$0,d2
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SpawnXYFromSelf - a4 op: sets the spawn position registers d4/d5 to the object's x/y $2E/$32 (16.16)
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: movem.l $2E(a6),d4-d5; script uses $35A00, $37022 (`63C0 63CA 607C`); 31 corpus runs
; ---------------------------------------------------------------------------
Op4_SpawnXYFromSelf:
	movem.l	$2E(a6),d4-d5
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ClearSpawnPolar - a4 op: clears the spawn radius d2 and heading d3 registers
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: moveq #0,d2 / moveq #0,d3; script uses $1E384, $37024; 29 corpus runs
; ---------------------------------------------------------------------------
Op4_ClearSpawnPolar:
	moveq	#$0,d2
	moveq	#$0,d3
	movea.w	(a4)+,a0
	jmp	(a0)
