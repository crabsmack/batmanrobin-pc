; ---------------------------------------------------------------------------
; Boss_HarleyJet_IsFlipped - jetpack Harley script test: returns 1 when her sprite is drawn H-flipped (bit 3 of the attr byte $38(a5)), else 0
; In:  a5 = her sprite block
; Out: d0 = 0 or 1
; Evidence: op $2CB2/$2D30 operands at $1F09C/$1F0A4 in Scr2_01EFCE point here; TAS f12385-f12627 (6 frames); like Boss_Harley_IsPartFlipped
; ---------------------------------------------------------------------------
Boss_HarleyJet_IsFlipped:
	btst	#$3,$38(a5)
	bne.s	loc_01F0D8
	moveq	#$0,d0
	rts
loc_01F0D8:
	moveq	#$1,d0
	rts
