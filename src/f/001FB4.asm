; ---------------------------------------------------------------------------
; Op2_BranchTerrainCode - a2 op: branches when the byte $50(a3) (high byte of the terrain routine's d3, stored by the move op) equals the operand's low byte
; In:  a2 = script: word code, word branch offset; a3 = motion block
; Out: branch or Op2_SkipWord
; Evidence: cmp.b $50(a3),d0; $4CDA stores d3 at $50; the meaning of d3 per terrain routine is not established (0 on the flat floors); 16 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_BranchTerrainCode:
	move.w	(a2)+,d0
	cmp.b	$50(a3),d0
	bne.w	Op2_SkipWord
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_UseMainTerrain - a2 op: makes the object collide with the scene's main terrain routine g_TerrainFn (clears bit 0 of (a3))
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: Coll_MoveScene picks g_TerrainFnAlt when bit 0 of (a3) is set; 94 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_UseMainTerrain:
	bclr	#$0,(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_UseAltTerrain - a2 op: makes the object collide with the scene's alternative terrain routine g_TerrainFnAlt (sets bit 0 of (a3))
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: Coll_MoveScene tests bit 0 of (a3); e.g. Coll_FlatFloorRest is the alt routine of a flat floor; 89 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_UseAltTerrain:
	bset	#$0,(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_ClearVar0 - a2 op: clears the object's script variable 0 (word $22(a4))
; In:  a2 = script (no operand); a4 = object's a4 block
; Out: NEXT
; Evidence: clr.w $22(a4); the player code $10322 stores the pad bits there, $11BC4 steps it as an index; 35 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_ClearVar0:
	clr.w	$22(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetVar0 - a2 op: sets script variable 0 ($22(a4)) to a word
; In:  a2 = script: word value; a4 = object's a4 block
; Out: NEXT
; Evidence: move.w (a2)+,$22(a4); 27 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetVar0:
	move.w	(a2)+,$22(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_OrVar0 - a2 op: sets bits in script variable 0 ($22(a4)): var |= word
; In:  a2 = script: word bits; a4 = object's a4 block
; Out: NEXT
; Evidence: or.w; 44 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_OrVar0:
	move.w	(a2)+,d0
	or.w	d0,$22(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AndVar0 - a2 op: masks script variable 0 ($22(a4)): var &= word
; In:  a2 = script: word mask; a4 = object's a4 block
; Out: NEXT
; Evidence: and.w; 38 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_AndVar0:
	move.w	(a2)+,d0
	and.w	d0,$22(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
