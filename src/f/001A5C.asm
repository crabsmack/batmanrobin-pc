; ---------------------------------------------------------------------------
; Op2_SeekScreenY4 - a2 op: Mot_SeekScreenY with a 4-pixel tolerance
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: moveq #4,d1 then bra $1A62; 18 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekScreenY4:
	moveq	#$4,d1
	bra.s	Mot_SeekScreenY
