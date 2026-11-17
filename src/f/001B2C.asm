; ---------------------------------------------------------------------------
; Op2_SeekWorldY8OrBranch - a2 op: Mot_SeekWorldYOrBranch with an 8-pixel tolerance
; In:  a2 = script: word branch offset; a3 = motion block
; Out: branch when arrived, else NEXT
; Evidence: moveq #8,d1 then bra $1B32; 11 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SeekWorldY8OrBranch:
	moveq	#$8,d1
	bra.s	Mot_SeekWorldYOrBranch
