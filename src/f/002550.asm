; ---------------------------------------------------------------------------
; Obj_SyncChild - copies parent fields to the child in an a6 field by flag bits 15-8: x/y $3E/$42 -> $3E/$42, drawn pos $4E/$50 -> $4E/$50, x/y -> drawn pos, drawn pos -> x/y, add word pair to child x/y, add word pair to child drawn pos, sprite attr $38(a5) and depth $3A(a5) to the child's a5
; In:  a2 = word a6 field offset, word flags, then a word pair for each of bits 11/10; a6/a5 = parent
; Out: d1 = child a6 (0 = none: the offset pairs are skipped); a2 past the operands
; Evidence: the bit-by-bit add.w d2,d2 / bcc chain; ops $2506/$2548 call it, $25E4 (never run) is the child-to-parent mirror; 34 runs
; ---------------------------------------------------------------------------
Obj_SyncChild:
	move.w	(a2)+,d0
	move.w	(a2)+,d2
	move.w	(a6,d0.w),d1
	bne.s	loc_002568
	lsl.w	#$5,d2
	bcc.s	loc_002560
	addq.l	#$4,a2
loc_002560:
	add.w	d2,d2
	bcc.s	loc_002566
	addq.l	#$4,a2
loc_002566:
	rts
loc_002568:
	movea.w	d1,a0
	add.w	d2,d2
	bcc.s	loc_00257A
	move.w	$3E(a6),$3E(a0)
	move.w	$42(a6),$42(a0)
loc_00257A:
	add.w	d2,d2
	bcc.s	loc_00258A
	move.w	$4E(a6),$4E(a0)
	move.w	$50(a6),$50(a0)
loc_00258A:
	add.w	d2,d2
	bcc.s	loc_00259A
	move.w	$3E(a6),$4E(a0)
	move.w	$42(a6),$50(a0)
loc_00259A:
	add.w	d2,d2
	bcc.s	loc_0025AA
	move.w	$4E(a6),$3E(a0)
	move.w	$50(a6),$42(a0)
loc_0025AA:
	add.w	d2,d2
	bcc.s	loc_0025BA
	move.w	(a2)+,d3
	add.w	d3,$3E(a0)
	move.w	(a2)+,d3
	add.w	d3,$42(a0)
loc_0025BA:
	add.w	d2,d2
	bcc.s	loc_0025CA
	move.w	(a2)+,d3
	add.w	d3,$4E(a0)
	move.w	(a2)+,d3
	add.w	d3,$50(a0)
loc_0025CA:
	movea.w	$C(a0),a0
	add.w	d2,d2
	bcc.s	loc_0025D8
	move.w	$38(a5),$38(a0)
loc_0025D8:
	add.w	d2,d2
	bcc.s	loc_0025E2
	move.w	$3A(a5),$3A(a0)
loc_0025E2:
	rts
