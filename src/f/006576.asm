; ---------------------------------------------------------------------------
; Op4_AltTerrainMoveOrFall - a4 op: moves $56/$58 to d4/d5 against the alternative terrain g_TerrainFnAlt (masks 9/$B) and branches when there is no floor contact (falling); on a floor continues
; In:  a4 = script: word branch offset; d4/d5 = drawn x/y words
; Out: NEXT or the branch; d4/d5 = corrected position
; Evidence: jsr (g_TerrainFnAlt) / tst.w d7 / beq Op4_Goto; script use $26D64 (`6576 0002 61FC`: then Op4_SetXYFromDraw); 3 corpus runs
; ---------------------------------------------------------------------------
Op4_AltTerrainMoveOrFall:
	move.w	#$9,d6
	move.w	#$B,d7
	move.w	d4,d1
	move.w	d5,d2
	movem.w	$56(a6),d4-d5
	sub.w	d4,d1
	sub.w	d5,d2
	movea.l	(g_TerrainFnAlt).w,a0
	jsr	(a0)
	movem.w	d4-d5,$56(a6)
	tst.w	d7
	beq.w	Op4_Goto
	addq.l	#$2,a4
	movea.w	(a4)+,a0
	jmp	(a0)
