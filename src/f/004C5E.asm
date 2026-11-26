; ---------------------------------------------------------------------------
; Op2_ApplyMoveBranchAtY - a2 op: adds this frame's move d1/d2 to x/y; when y reaches a word limit (from either side) it is clamped there and the script branches
; In:  a2 = script: word limit (y, whole pixels), word branch offset; d1/d2 = move (16.16)
; Out: $3E(a6) += d1; $42(a6) += d2, or = limit << 16 and a branch; else NEXT past the offset (Op2_SkipWord)
; Evidence: add.l d1,$3E(a6) / limit = word << 16 / cmp.l $42(a6) before and after add.l d2 / move.l d0,$42(a6) / adda.w (a2),a2; Scr2_01DEB6 at $01E044 (`4B74 50E6 000E 4C5E 00B4`); never run in the corpus; named at a script op position no corpus run read (tools/raw_reach.py, R27)
; ---------------------------------------------------------------------------
Op2_ApplyMoveBranchAtY:
	add.l	d1,$3E(a6)
	moveq	#$0,d0
	move.w	(a2)+,d0
	swap	d0
	cmp.l	$42(a6),d0
	bgt.s	loc_004C84
	add.l	d2,$42(a6)
	cmp.l	$42(a6),d0
	blt.w	Op2_SkipWord
	move.l	d0,$42(a6)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_004C84:
	add.l	d2,$42(a6)
	cmp.l	$42(a6),d0
	bgt.w	Op2_SkipWord
	move.l	d0,$42(a6)
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
