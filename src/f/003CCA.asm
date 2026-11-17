; ---------------------------------------------------------------------------
; Op2_SwitchAimDir - a2 op: branches through a table of word offsets indexed by the sprite's aim direction ($1A(a5) / 4)
; In:  a2 = script: one word offset per direction (each relative to itself)
; Out: NEXT at the chosen case
; Evidence: lsr.w #1 / andi #$FFFE / adda d0,a2 / adda.w (a2),a2; 14 corpus runs; $1C25A `3CCA 0012 0018 001E ...`
; ---------------------------------------------------------------------------
Op2_SwitchAimDir:
	move.w	$1A(a5),d0
	lsr.w	#$1,d0
	andi.w	#$FFFE,d0
	adda.w	d0,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
