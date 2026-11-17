; ---------------------------------------------------------------------------
; Op2_SeekWorldX8OrBranch - a2 op: Mot_SeekWorldXOrBranch with an 8-pixel tolerance
; In:  a2 = script: word branch offset; a3 = motion block
; Out: branch when arrived, else NEXT
; Evidence: moveq #8,d1 then bra $19F2; 14 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekWorldX8OrBranch:
	moveq	#$8,d1
	bra.s	Mot_SeekWorldXOrBranch
