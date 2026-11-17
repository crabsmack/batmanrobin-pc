; ---------------------------------------------------------------------------
; Coll_WalkBlockMap - terrain move for a grounded mover: walks d1 pixels along the block map, climbing or descending one pixel per step to follow the floor, then probes the pixel below
; In:  d1 = x move, d4/d5 = position, d6/d7 = wall/floor masks
; Out: d4/d5, d3 as Coll_BlockMapResult; d6 = wall mask hit (0 none), d7 = floor mask under the feet (0 = left the ground)
; Evidence: the bit-0-of-(a3) half of Coll_MoveOnBlockMap; 12k TAS frames (6362-101674) and scenario-level-skip; with d1 = 0 it only tests one pixel down
; ---------------------------------------------------------------------------
Coll_WalkBlockMap:
	lea.l	(g_LayerA).l,a6
	movea.l	$C(a6),a3
	move.w	d4,d0
	swap	d4
	asr.w	#$4,d0
	move.w	d0,d4
	bmi.s	loc_0120C6
	cmp.w	$2E(a6),d4
	bcs.s	loc_0120C2
	move.w	$2E(a6),d0
	subq.w	#$1,d0
loc_0120C2:
	add.w	d0,d0
	adda.w	d0,a3
loc_0120C6:
	swap	d4
	add.w	d4,d4
	andi.w	#$1E,d4
	move.w	d5,d0
	swap	d5
	asr.w	#$4,d0
	move.w	d0,d5
	bmi.s	loc_0120EA
	cmp.w	$30(a6),d5
	bcs.s	loc_0120E4
	move.w	$30(a6),d0
	subq.w	#$1,d0
loc_0120E4:
	mulu.w	$2A(a6),d0
	adda.l	d0,a3
loc_0120EA:
	swap	d5
	andi.w	#$F,d5
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
	tst.w	d1
	beq.w	Coll_WalkBlockMapEnd
	bmi.s	loc_012114
	subq.w	#$1,d1
	bra.w	Coll_WalkBlockMapRight
loc_012114:
	neg.w	d1
	subq.w	#$1,d1
	bra.w	Coll_WalkBlockMapLeft
; ---------------------------------------------------------------------------
; Coll_WalkBlockMapEnd - end of the floor-following walk: clears the wall result, then probes the pixel below (Coll_WalkBlockMapProbe)
; In:  the block-map registers of Coll_WalkBlockMap
; Out: d6 = 0; as Coll_WalkBlockMapProbe
; Evidence: moveq #0,d6 falling into $1211E; branched to by Coll_WalkBlockMap (d1 = 0) and by the walk loops when their pixels are used up
; ---------------------------------------------------------------------------
Coll_WalkBlockMapEnd:
	moveq	#$0,d6
; ---------------------------------------------------------------------------
; Coll_WalkBlockMapProbe - probes the pixel below the walker (down one, test, back up): d7 = the floor mask there, 0 when no floor; then Coll_BlockMapResult
; In:  the block-map registers of Coll_WalkBlockMap, d7 = floor mask
; Out: d7 = floor under the feet (0 none); d4/d5/d3 as Coll_BlockMapResult
; Evidence: addq.w #1,d5 ... move.w (a0),d0 / and.w d0,d7 / the [top,bottom] test, back up a pixel on both paths, bra Coll_BlockMapResult; branched to by the walk loops after a wall step
; ---------------------------------------------------------------------------
Coll_WalkBlockMapProbe:
	addq.w	#$1,d5
	andi.w	#$F,d5
	bne.s	loc_01214E
	swap	d5
	addq.w	#$1,d5
	ble.s	loc_01214C
	cmp.w	$30(a6),d5
	bge.s	loc_01214C
	adda.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_01214C:
	swap	d5
loc_01214E:
	move.w	(a0),d0
	and.w	d0,d7
	beq.w	loc_01219C
	cmp.b	$2(a0,d4.w),d5
	bcs.w	loc_01219C
	cmp.b	$3(a0,d4.w),d5
	bgt.w	loc_01219C
	subq.w	#$1,d5
	bcc.s	loc_012198
	andi.w	#$F,d5
	swap	d5
	cmp.w	$30(a6),d5
	bge.s	loc_012194
	tst.w	d5
	ble.s	loc_012194
	suba.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_012194:
	subq.w	#$1,d5
	swap	d5
loc_012198:
	bra.w	Coll_BlockMapResult
loc_01219C:
	subq.w	#$1,d5
	bcc.s	loc_0121CE
	andi.w	#$F,d5
	swap	d5
	cmp.w	$30(a6),d5
	bge.s	loc_0121CA
	tst.w	d5
	ble.s	loc_0121CA
	suba.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_0121CA:
	subq.w	#$1,d5
	swap	d5
loc_0121CE:
	moveq	#$0,d7
	bra.w	Coll_BlockMapResult
; ---------------------------------------------------------------------------
; Coll_WalkBlockMapRight - the rightward stepping loop of the floor-following walk (also finishes a horizontal move after Coll_MoveBlockMap stops vertically)
; In:  d1 = pixels left - 1, the block-map registers of Coll_WalkBlockMap
; Out: as Coll_WalkBlockMap
; Evidence: entered from Coll_WalkBlockMap (d1 > 0) and from Coll_MoveBlockMap when g_CollMoveDX >= 0; executed in 45 corpus runs
; ---------------------------------------------------------------------------
Coll_WalkBlockMapRight:
	addq.w	#$2,d4
	andi.w	#$1E,d4
	bne.s	loc_012202
	swap	d4
	addq.w	#$1,d4
	ble.s	loc_012200
	cmp.w	$2E(a6),d4
	bge.s	loc_012200
	addq.l	#$2,a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_012200:
	swap	d4
loc_012202:
	move.w	(a0),d0
	and.w	d6,d0
	beq.w	loc_0122CA
	cmp.b	$2(a0,d4.w),d5
	bcs.w	loc_0122CA
	cmp.b	$3(a0,d4.w),d5
	bgt.w	loc_0122CA
	subq.w	#$1,d5
	bcc.s	loc_01224C
	andi.w	#$F,d5
	swap	d5
	cmp.w	$30(a6),d5
	bge.s	loc_012248
	tst.w	d5
	ble.s	loc_012248
	suba.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_012248:
	subq.w	#$1,d5
	swap	d5
loc_01224C:
	move.w	(a0),d0
	and.w	d6,d0
	beq.w	loc_01233E
	cmp.b	$2(a0,d4.w),d5
	bcs.w	loc_01233E
	cmp.b	$3(a0,d4.w),d5
	bgt.w	loc_01233E
	move.w	d0,d6
	subq.w	#$2,d4
	bcc.s	loc_012296
	andi.w	#$1E,d4
	swap	d4
	cmp.w	$2E(a6),d4
	bge.s	loc_012292
	tst.w	d4
	ble.s	loc_012292
	subq.l	#$2,a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_012292:
	subq.w	#$1,d4
	swap	d4
loc_012296:
	addq.w	#$1,d5
	andi.w	#$F,d5
	bne.s	loc_0122C6
	swap	d5
	addq.w	#$1,d5
	ble.s	loc_0122C4
	cmp.w	$30(a6),d5
	bge.s	loc_0122C4
	adda.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_0122C4:
	swap	d5
loc_0122C6:
	bra.w	Coll_WalkBlockMapProbe
loc_0122CA:
	addq.w	#$1,d5
	andi.w	#$F,d5
	bne.s	loc_0122FA
	swap	d5
	addq.w	#$1,d5
	ble.s	loc_0122F8
	cmp.w	$30(a6),d5
	bge.s	loc_0122F8
	adda.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_0122F8:
	swap	d5
loc_0122FA:
	move.w	(a0),d0
	and.w	d7,d0
	beq.s	loc_012346
	cmp.b	$2(a0,d4.w),d5
	bcs.s	loc_012346
	cmp.b	$3(a0,d4.w),d5
	bgt.s	loc_012346
loc_01230C:
	subq.w	#$1,d5
	bcc.s	loc_01233E
	andi.w	#$F,d5
	swap	d5
	cmp.w	$30(a6),d5
	bge.s	loc_01233A
	tst.w	d5
	ble.s	loc_01233A
	suba.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_01233A:
	subq.w	#$1,d5
	swap	d5
loc_01233E:
	dbra	d1,Coll_WalkBlockMapRight
	bra.w	Coll_WalkBlockMapEnd
loc_012346:
	addq.w	#$1,d5
	andi.w	#$F,d5
	bne.s	loc_012376
	swap	d5
	addq.w	#$1,d5
	ble.s	loc_012374
	cmp.w	$30(a6),d5
	bge.s	loc_012374
	adda.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_012374:
	swap	d5
loc_012376:
	move.w	(a0),d0
	and.w	d7,d0
	beq.s	loc_01233E
	cmp.b	$2(a0,d4.w),d5
	bcs.s	loc_01233E
	cmp.b	$3(a0,d4.w),d5
	bgt.s	loc_01233E
	bra.s	loc_01230C
; ---------------------------------------------------------------------------
; Coll_WalkBlockMapLeft - the leftward stepping loop of the floor-following walk (also finishes a horizontal move after Coll_MoveBlockMap stops vertically)
; In:  d1 = pixels left - 1, the block-map registers of Coll_WalkBlockMap
; Out: as Coll_WalkBlockMap
; Evidence: entered from Coll_WalkBlockMap (d1 < 0) and from Coll_MoveBlockMap when g_CollMoveDX < 0; executed in 43 corpus runs
; ---------------------------------------------------------------------------
Coll_WalkBlockMapLeft:
	subq.w	#$2,d4
	bcc.s	loc_0123BA
	andi.w	#$1E,d4
	swap	d4
	cmp.w	$2E(a6),d4
	bge.s	loc_0123B6
	tst.w	d4
	ble.s	loc_0123B6
	subq.l	#$2,a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_0123B6:
	subq.w	#$1,d4
	swap	d4
loc_0123BA:
	move.w	(a0),d0
	and.w	d6,d0
	beq.w	loc_012480
	cmp.b	$2(a0,d4.w),d5
	bcs.w	loc_012480
	cmp.b	$3(a0,d4.w),d5
	bgt.w	loc_012480
	subq.w	#$1,d5
	bcc.s	loc_012404
	andi.w	#$F,d5
	swap	d5
	cmp.w	$30(a6),d5
	bge.s	loc_012400
	tst.w	d5
	ble.s	loc_012400
	suba.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_012400:
	subq.w	#$1,d5
	swap	d5
loc_012404:
	move.w	(a0),d0
	and.w	d6,d0
	beq.w	loc_0124F4
	cmp.b	$2(a0,d4.w),d5
	bcs.w	loc_0124F4
	cmp.b	$3(a0,d4.w),d5
	bgt.w	loc_0124F4
	move.w	d0,d6
	addq.w	#$2,d4
	andi.w	#$1E,d4
	bne.s	loc_01244C
	swap	d4
	addq.w	#$1,d4
	ble.s	loc_01244A
	cmp.w	$2E(a6),d4
	bge.s	loc_01244A
	addq.l	#$2,a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_01244A:
	swap	d4
loc_01244C:
	addq.w	#$1,d5
	andi.w	#$F,d5
	bne.s	loc_01247C
	swap	d5
	addq.w	#$1,d5
	ble.s	loc_01247A
	cmp.w	$30(a6),d5
	bge.s	loc_01247A
	adda.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_01247A:
	swap	d5
loc_01247C:
	bra.w	Coll_WalkBlockMapProbe
loc_012480:
	addq.w	#$1,d5
	andi.w	#$F,d5
	bne.s	loc_0124B0
	swap	d5
	addq.w	#$1,d5
	ble.s	loc_0124AE
	cmp.w	$30(a6),d5
	bge.s	loc_0124AE
	adda.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_0124AE:
	swap	d5
loc_0124B0:
	move.w	(a0),d0
	and.w	d7,d0
	beq.s	loc_0124FC
	cmp.b	$2(a0,d4.w),d5
	bcs.s	loc_0124FC
	cmp.b	$3(a0,d4.w),d5
	bgt.s	loc_0124FC
loc_0124C2:
	subq.w	#$1,d5
	bcc.s	loc_0124F4
	andi.w	#$F,d5
	swap	d5
	cmp.w	$30(a6),d5
	bge.s	loc_0124F0
	tst.w	d5
	ble.s	loc_0124F0
	suba.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_0124F0:
	subq.w	#$1,d5
	swap	d5
loc_0124F4:
	dbra	d1,Coll_WalkBlockMapLeft
	bra.w	Coll_WalkBlockMapEnd
loc_0124FC:
	addq.w	#$1,d5
	andi.w	#$F,d5
	bne.s	loc_01252C
	swap	d5
	addq.w	#$1,d5
	ble.s	loc_01252A
	cmp.w	$30(a6),d5
	bge.s	loc_01252A
	adda.w	$2A(a6),a3
	move.w	(a3),d0
	lsr.w	#$1,d0
	movea.l	$14(a6),a0
	move.w	$2(a0,d0.w),(g_CollBlockAttr).w
	movea.w	(a0,d0.w),a0
	adda.l	$18(a6),a0
loc_01252A:
	swap	d5
loc_01252C:
	move.w	(a0),d0
	and.w	d7,d0
	beq.s	loc_0124F4
	cmp.b	$2(a0,d4.w),d5
	bcs.s	loc_0124F4
	cmp.b	$3(a0,d4.w),d5
	bgt.s	loc_0124F4
	bra.s	loc_0124C2
