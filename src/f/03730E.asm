; ---------------------------------------------------------------------------
; ToyChain_PlaceAndDrawLinks - places and draws a chain's links: from the drawn base $4E/$50(a6) each link sits 20 px further along a heading that starts at +$4 + (sway x>>8) * +$8 and bends with the sway from link to link (var 0 bit 0 picks one of two bend modes), drawn rotated by heading + $2E through Tbl_036DA8 or the outlined frames (charged / blinking), positions kept in each link's +$44/+$46 and the tip in +$30/+$32
; In:  a2 = chain block, a6 = chain object ($3E.l = sway value moved by the a3 motion ops), a4 +$22 var 0 / +$24 count, a5 = sprite block (attr $38, depth $3A)
; Out: g_VM2Arg0 = base heading; link frames swapped and sprites added; chain block +$1A = this frame's blink mask, +$30/+$32 = tip x/y
; Evidence: 20-px steps (addi #$14), Tbl_059B22 sine/cosine, mulu #$F00 (heading -> 0-59), rol.w $1A(a2) selects Tbl_ToyFigureChargedFrames; g_FrameCounter low word bit 3 + Rand_Next for the blink; TAS f79928-f112311 (6555 frames); screenshots f84000/f84600: a swaying column of linked toy figures
; ---------------------------------------------------------------------------
ToyChain_PlaceAndDrawLinks:
	lea.l	$34(a2),a3
	move.l	$3E(a6),d3
	move.l	d3,d2
	asr.l	#$8,d2
	muls.w	$8(a2),d2
	add.l	(a2),d3
	add.l	$4(a2),d2
	move.l	d2,(g_VM2Arg0).w
	move.l	d3,d6
	move.w	$4E(a6),d4
	move.w	$50(a6),d5
	swap	d4
	swap	d5
	move.w	#$8000,d4
	move.w	#$8000,d5
	move.w	$24(a4),d7
	subq.w	#$1,d7
	bmi.w	loc_03747A
	move.w	$22(a4),d0
	andi.w	#$C,d0
	beq.s	loc_037378
	andi.w	#$4,d0
	bne.s	loc_037374
	move.w	(g_FrameCounter+2).w,d0
	andi.w	#$8,d0
	beq.s	loc_037378
	jsr	(Rand_Next).l
	andi.w	#$1,d0
	beq.s	loc_037378
	move.w	#$FFFF,d0
	bra.s	loc_037378
loc_037374:
	move.w	$18(a2),d0
loc_037378:
	move.w	d0,$1A(a2)
	move.w	$C(a2),d0
loc_037380:
	addi.w	#$14,d0
	dbge	d7,loc_037380
	blt.w	loc_03747A
	subi.w	#$14,d0
	add.w	d0,d0
	divs.w	#$A0,d3
	muls.w	d0,d3
	asl.l	#$2,d3
	add.l	d6,d3
	move.l	d2,d1
	swap	d1
	andi.l	#$3FE,d1
	lea.l	(Tbl_059B22).l,a0
	adda.w	d1,a0
	move.w	$100(a0),d1
	muls.w	d0,d1
	add.l	d1,d4
	move.w	(a0),d1
	muls.w	d0,d1
	add.l	d1,d5
	move.l	d2,d0
	move.w	$22(a4),d1
	andi.w	#$1,d1
	bne.s	loc_0373D0
	sub.l	d6,d3
	add.l	d3,d2
	move.l	d6,d3
	moveq	#$0,d6
loc_0373D0:
	swap	d0
	andi.w	#$3FE,d0
	lea.l	(Tbl_059B22).l,a0
	adda.w	d0,a0
	move.w	$100(a0),d1
	muls.w	#$14,d1
	add.l	d1,d4
	add.l	d4,d1
	move.l	d1,-(a7)
	move.w	(a0),d1
	muls.w	#$14,d1
	add.l	d1,d5
	add.l	d5,d1
	move.l	d1,-(a7)
	movea.w	(a3)+,a1
	movem.l	d2-d3/d6-d7,-(a7)
	swap	d4
	swap	d5
	move.w	d4,$44(a1)
	move.w	d5,$46(a1)
	add.w	$2E(a2),d0
	andi.w	#$3FE,d0
	mulu.w	#$F00,d0
	swap	d0
	add.w	d0,d0
	add.w	d0,d0
	lea.l	Tbl_036DA8(pc),a0
	rol.w	$1A(a2)
	bcc.s	loc_03742A
	lea.l	Tbl_ToyFigureChargedFrames(pc),a0
loc_03742A:
	adda.w	d0,a0
	move.w	(a0)+,d1
	move.b	$38(a5),d6
	and.b	(a0)+,d6
	move.b	(a0)+,d0
	eor.b	d0,d6
	lsl.w	#$8,d6
	move.w	$42(a1),d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,$42(a1)
	move.w	$3A(a5),d7
	bne.s	loc_037456
	jsr	(Spr_AddBack).l
	bra.s	loc_03745C
loc_037456:
	jsr	(Spr_AddSorted).l
loc_03745C:
	movem.l	(a7)+,d2-d3/d6-d7
	move.l	(a7)+,d5
	move.l	(a7)+,d4
	add.l	d3,d2
	add.l	d6,d3
	move.l	d2,d0
	dbra	d7,loc_0373D0
	swap	d4
	swap	d5
	move.w	d4,$30(a2)
	move.w	d5,$32(a2)
loc_03747A:
	rts
