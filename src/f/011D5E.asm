; ---------------------------------------------------------------------------
; Coll_MoveBlockMap - terrain move through plane A's 16x16 block collision map: steps the point pixel by pixel (Bresenham, x and y), stopping each axis at the first solid pixel
; In:  d1/d2 = x/y move in pixels, d4/d5 = position, d6/d7 = wall/floor solidity masks (moving up only bit 0 counts)
; Out: d4/d5 = final position, d6 = the block mask hit sideways (0 none), d7 = the mask hit vertically (0 none), d3 = the last block's attribute word (g_CollBlockAttr); a vertical stop with x left over finishes in Coll_WalkBlockMapRight/Left
; Evidence: the (a3)-bit-0-clear half of Coll_MoveOnBlockMap (bit 0 = on the ground, inferred from Coll_WalkBlockMap); g_LayerA +$C map, +$14 block table, +$18 collision data, +$2A row bytes, +$2E/+$30 width/height in blocks; a block's data is a mask word then for each of the 16 pixel columns a solid [top, bottom] byte pair; 19k TAS frames (6363-101432)
; ---------------------------------------------------------------------------
Coll_MoveBlockMap:
	move.w	d1,d0
	or.w	d2,d0
	bne.s	loc_011D6C
	moveq	#$0,d3
	moveq	#$0,d6
	moveq	#$0,d7
	rts
loc_011D6C:
	move.w	d1,(g_CollMoveDX).w
	lea.l	(g_LayerA).l,a6
	movea.l	$C(a6),a3
	move.w	d4,d0
	swap	d4
	asr.w	#$4,d0
	move.w	d0,d4
	ble.s	loc_011D94
	cmp.w	$2E(a6),d4
	bcs.s	loc_011D90
	move.w	$2E(a6),d0
	subq.w	#$1,d0
loc_011D90:
	add.w	d0,d0
	adda.w	d0,a3
loc_011D94:
	swap	d4
	add.w	d4,d4
	andi.w	#$1E,d4
	move.w	d5,d0
	swap	d5
	asr.w	#$4,d0
	move.w	d0,d5
	ble.s	loc_011DB8
	cmp.w	$30(a6),d5
	bcs.s	loc_011DB2
	move.w	$30(a6),d0
	subq.w	#$1,d0
loc_011DB2:
	mulu.w	$2A(a6),d0
	adda.l	d0,a3
loc_011DB8:
	swap	d5
	andi.w	#$F,d5
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
	tst.w	d1
	bpl.s	loc_011DE2
	neg.w	d1
	lea.l	(loc_011EDA).l,a5
	bra.s	loc_011DE8
loc_011DE2:
	lea.l	(loc_011E4E).l,a5
loc_011DE8:
	tst.w	d2
	bpl.s	loc_011DFE
	neg.w	d2
	lea.l	(loc_012004).l,a4
	andi.w	#$1,d7
	andi.w	#$1,d6
	bra.s	loc_011E04
loc_011DFE:
	lea.l	(loc_011F66).l,a4
loc_011E04:
	movea.w	d1,a2
	movea.w	d2,a1
	dbra	d1,loc_011E0E
	clr.w	d6
loc_011E0E:
	dbra	d2,loc_011E14
	clr.w	d7
loc_011E14:
	move.l	a5,(g_CollStepX).w
	move.l	a4,(g_CollStepY).w
	cmpa.w	a1,a2
	bgt.s	loc_011E24
	exg.l	a4,a5
	exg.l	a1,a2
loc_011E24:
	move.w	a2,d3
	adda.w	a2,a2
	adda.w	d3,a2
	adda.w	a1,a1
; ---------------------------------------------------------------------------
; Coll_BlockMapStep - one Bresenham step of Coll_MoveBlockMap: takes the minor axis's error off d3 and jumps to the major (a5) or minor (a4) axis step
; In:  d3 = error, a1/a2 = the error steps, a4/a5 = the axis step routines, the block-map registers
; Out: d3 updated, then the chosen step (its loop returns here until both moves are used up)
; Evidence: sub.w a1,d3 / bpl / add.w a2,d3 / jmp (a4) / jmp (a5); the dbra target of the four step routines $11E4E/$11EDA/$11F66/$12004
; ---------------------------------------------------------------------------
Coll_BlockMapStep:
	sub.w	a1,d3
	bpl.s	loc_011E34
	add.w	a2,d3
	jmp	(a4)
loc_011E34:
	jmp	(a5)
; ---------------------------------------------------------------------------
; Coll_BlockMapResult - common exit of the block-map moves: turns block/pixel positions back into pixels and returns the block attribute
; In:  d4/d5 = block index (high) : pixel offset (low)
; Out: d4/d5 pixels, d3 = g_CollBlockAttr
; Evidence: branched to from Coll_MoveBlockMap and Coll_WalkBlockMap (9 references)
; ---------------------------------------------------------------------------
Coll_BlockMapResult:
	move.l	d4,d0
	swap	d0
	asl.w	#$4,d0
	asr.w	#$1,d4
	add.w	d0,d4
	move.l	d5,d0
	swap	d0
	asl.w	#$4,d0
	add.w	d0,d5
	move.w	(g_CollBlockAttr).w,d3
	rts
loc_011E4E:
	addq.w	#$2,d4
	andi.w	#$1E,d4
	bne.s	loc_011E7C
	swap	d4
	addq.w	#$1,d4
	ble.s	loc_011E7A
	cmp.w	$2E(a6),d4
	bge.s	loc_011E7A
	addq.l	#$2,a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_011E7A:
	swap	d4
loc_011E7C:
	move.w	(a0),d0
	and.w	d6,d0
	beq.s	loc_011EC6
	cmp.b	$2(a0,d4.w),d5
	bcs.s	loc_011EC6
	cmp.b	$3(a0,d4.w),d5
	bgt.s	loc_011EC6
	move.w	d0,d6
	moveq	#-$1,d1
	subq.w	#$2,d4
	bcc.s	loc_011EC2
	andi.w	#$1E,d4
	swap	d4
	cmp.w	$2E(a6),d4
	bge.s	loc_011EBE
	tst.w	d4
	ble.s	loc_011EBE
	subq.l	#$2,a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_011EBE:
	subq.w	#$1,d4
	swap	d4
loc_011EC2:
	bra.w	loc_011ECC
loc_011EC6:
	dbra	d1,Coll_BlockMapStep
	moveq	#$0,d6
loc_011ECC:
	tst.w	d2
	bmi.w	Coll_BlockMapResult
	movea.l	(g_CollStepY).w,a5
	movea.l	a5,a4
	jmp	(a5)
loc_011EDA:
	subq.w	#$2,d4
	bcc.s	loc_011F0A
	andi.w	#$1E,d4
	swap	d4
	cmp.w	$2E(a6),d4
	bge.s	loc_011F06
	tst.w	d4
	ble.s	loc_011F06
	subq.l	#$2,a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_011F06:
	subq.w	#$1,d4
	swap	d4
loc_011F0A:
	move.w	(a0),d0
	and.w	d6,d0
	beq.s	loc_011F52
	cmp.b	$2(a0,d4.w),d5
	bcs.s	loc_011F52
	cmp.b	$3(a0,d4.w),d5
	bgt.s	loc_011F52
	move.w	d0,d6
	moveq	#-$1,d1
	addq.w	#$2,d4
	andi.w	#$1E,d4
	bne.s	loc_011F4E
	swap	d4
	addq.w	#$1,d4
	ble.s	loc_011F4C
	cmp.w	$2E(a6),d4
	bge.s	loc_011F4C
	addq.l	#$2,a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_011F4C:
	swap	d4
loc_011F4E:
	bra.w	loc_011F58
loc_011F52:
	dbra	d1,Coll_BlockMapStep
	moveq	#$0,d6
loc_011F58:
	tst.w	d2
	bmi.w	Coll_BlockMapResult
	movea.l	(g_CollStepY).w,a5
	movea.l	a5,a4
	jmp	(a5)
loc_011F66:
	addq.w	#$1,d5
	andi.w	#$F,d5
	bne.s	loc_011F96
	swap	d5
	addq.w	#$1,d5
	ble.s	loc_011F94
	cmp.w	$30(a6),d5
	bge.s	loc_011F94
	adda.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_011F94:
	swap	d5
loc_011F96:
	move.w	(a0),d0
	and.w	d7,d0
	beq.s	loc_011FF0
	cmp.b	$2(a0,d4.w),d5
	bcs.s	loc_011FF0
	cmp.b	$3(a0,d4.w),d5
	bgt.s	loc_011FF0
	move.w	d0,d7
	moveq	#-$1,d2
	subq.w	#$1,d5
	bcc.s	loc_011FDE
	andi.w	#$F,d5
	swap	d5
	cmp.w	$30(a6),d5
	bge.s	loc_011FDA
	tst.w	d5
	ble.s	loc_011FDA
	suba.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_011FDA:
	subq.w	#$1,d5
	swap	d5
loc_011FDE:
	tst.w	d1
	bmi.w	Coll_BlockMapResult
	tst.w	(g_CollMoveDX).w
	bpl.w	Coll_WalkBlockMapRight
	bra.w	Coll_WalkBlockMapLeft
loc_011FF0:
	dbra	d2,Coll_BlockMapStep
	moveq	#$0,d7
	tst.w	d1
	bmi.w	Coll_BlockMapResult
	movea.l	(g_CollStepX).w,a5
	movea.l	a5,a4
	jmp	(a5)
loc_012004:
	subq.w	#$1,d5
	bcc.s	loc_012036
	andi.w	#$F,d5
	swap	d5
	cmp.w	$30(a6),d5
	bge.s	loc_012032
	tst.w	d5
	ble.s	loc_012032
	suba.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_012032:
	subq.w	#$1,d5
	swap	d5
loc_012036:
	move.w	(a0),d0
	and.w	d7,d0
	beq.s	loc_01208E
	cmp.b	$2(a0,d4.w),d5
	bcs.s	loc_01208E
	cmp.b	$3(a0,d4.w),d5
	bgt.s	loc_01208E
	move.w	d0,d7
	moveq	#-$1,d2
	addq.w	#$1,d5
	andi.w	#$F,d5
	bne.s	loc_01207C
	swap	d5
	addq.w	#$1,d5
	ble.s	loc_01207A
	cmp.w	$30(a6),d5
	bge.s	loc_01207A
	adda.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_01207A:
	swap	d5
loc_01207C:
	tst.w	d1
	bmi.w	Coll_BlockMapResult
	tst.w	(g_CollMoveDX).w
	bpl.w	Coll_WalkBlockMapRight
	bra.w	Coll_WalkBlockMapLeft
loc_01208E:
	dbra	d2,Coll_BlockMapStep
	moveq	#$0,d7
	tst.w	d1
	bmi.w	Coll_BlockMapResult
	movea.l	(g_CollStepX).w,a5
	movea.l	a5,a4
	jmp	(a5)
