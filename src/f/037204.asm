; ---------------------------------------------------------------------------
; ToyChain_Grow - grows a chain: the extension +$C advances by the grow speed +$10 and a link is added each time it reaches 0 (then minus 20 px), first for the initial links +$2A, after them at the second speed +$14 back up to the maximum +$2C (regrowing links lost to hits)
; In:  a2 = chain block, a4 = chain a4 block (+$24 = links now), a6 = chain object
; Out: chain block +$C/+$10/+$2A updated; links added by ToyChain_AddLink (extension cleared when the pool is full or the chain is complete)
; Evidence: add.l $10(a2),$C(a2) / subi #$14 / subq #1,$2A(a2) / move.l $14(a2),$10(a2) / cmp $2C(a2); TAS f79928-f112311 with the chain update
; ---------------------------------------------------------------------------
ToyChain_Grow:
	move.l	$10(a2),d0
	beq.s	loc_03724E
	add.l	d0,$C(a2)
	bmi.s	loc_03725E
	tst.w	$2A(a2)
	beq.s	loc_037230
	bsr.w	ToyChain_AddLink
	bne.s	loc_03723A
	subi.w	#$14,$C(a2)
	subq.w	#$1,$2A(a2)
	bne.s	loc_03725E
	move.l	$14(a2),$10(a2)
	rts
loc_037230:
	move.w	$24(a4),d0
	cmp.w	$2C(a2),d0
	blt.s	loc_037240
loc_03723A:
	clr.l	$C(a2)
	rts
loc_037240:
	bsr.w	ToyChain_AddLink
	bne.s	loc_03723A
	subi.w	#$14,$C(a2)
	rts
loc_03724E:
	tst.w	$2A(a2)
	beq.s	loc_03725E
	bsr.w	ToyChain_AddLink
	bne.s	loc_03725E
	subq.w	#$1,$2A(a2)
loc_03725E:
	rts
