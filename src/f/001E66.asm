; ---------------------------------------------------------------------------
; Op2_SetTerrainBoxB - a2 op: sets and enables terrain box B (bit 2 of (a3), collided first by the move op): offset $48/$4A, wall and floor masks $4C/$4E, contact results cleared
; In:  a2 = script: word dx, word dy, word wall mask, word floor mask; a3 = motion block
; Out: NEXT
; Evidence: $4CDA: with bit 2 set it collides box B first and uses box A only when B found no floor; 8 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetTerrainBoxB:
	clr.w	$52(a3)
	clr.w	$54(a3)
	clr.w	$50(a3)
; ---------------------------------------------------------------------------
; Op2_SetTerrainBoxBKeepResults - a2 op: sets terrain box B's offset ($48/$4A) and wall/floor masks ($4C/$4E) and turns it on, the contact results $50-$54 kept (Op2_SetTerrainBoxB past its clears)
; In:  a2 = script: word dx, word dy, word wall mask, word floor mask; a3 = motion block
; Out: NEXT
; Evidence: a computed-jump target in the corpus: the NEXT of Op2_SetTerrainBoxATouching ($1DF6), 507 call-site log entries
; ---------------------------------------------------------------------------
Op2_SetTerrainBoxBKeepResults:
	move.w	(a2)+,$48(a3)
	move.w	(a2)+,$4A(a3)
	move.w	(a2)+,$4C(a3)
	move.w	(a2)+,$4E(a3)
; ---------------------------------------------------------------------------
; Op2_EnableTerrainBoxB - a2 op: turns terrain box B on (bset #2,(a3)) with its offset, masks and contact results as they are
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: a computed-jump target in the corpus: NEXT dispatches from Op2_Goto ($2D26), $1C68, $1E1A, ~20k call-site log entries
; ---------------------------------------------------------------------------
Op2_EnableTerrainBoxB:
	bset	#$2,(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DisableTerrainBoxB - a2 op: turns terrain box B off (bclr #2,(a3))
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: $4CDA tests bit 2 of (a3); 89 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_DisableTerrainBoxB:
	bclr	#$2,(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTerrainBoxBMasks - a2 op: changes terrain box B's wall and floor masks ($4C/$4E(a3)) only
; In:  a2 = script: word wall mask, word floor mask; a3 = motion block
; Out: NEXT
; Evidence: two word moves; 5 fuzz corpus runs (not the TAS)
; ---------------------------------------------------------------------------
Op2_SetTerrainBoxBMasks:
	move.w	(a2)+,$4C(a3)
	move.w	(a2)+,$4E(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
