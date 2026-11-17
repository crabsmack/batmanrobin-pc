; ---------------------------------------------------------------------------
; Layer_QueueColumn - steps the layer's block column by d0 and queues one column of 2x2-cell blocks (g_ScreenBlocksY tall, wrapping in the map and the plane) into the tile queue
; In:  a6 = layer, d0 = block column step, d1 = column offset to draw, d2 = plane cell column step, d3 = cell offset
; Out: +$26/+$32 updated; g_TileQueue entry: word -(8 x count) jump offset, VDP write command, then per cell row a control word and a long (2 cells); g_TileQueuePtr advanced
; Evidence: callers Layer_ScrollStepX, Layer_DrawScreen, the scene-script op $7416; cells = block tiles +$10 (2 longs per block) + the tile word +8
; ---------------------------------------------------------------------------
Layer_QueueColumn:
	move.w	$2E(a6),d4
	add.w	$26(a6),d0
	bpl.s	loc_012A90
	add.w	d4,d0
loc_012A90:
	cmp.w	d4,d0
	blt.s	loc_012A96
	sub.w	d4,d0
loc_012A96:
	move.w	d0,$26(a6)
	add.w	d1,d0
	bpl.s	loc_012AA4
loc_012A9E:
	add.w	d4,d0
	bpl.s	loc_012AAE
	bra.s	loc_012A9E
loc_012AA4:
	cmp.w	d4,d0
	blt.s	loc_012AAE
loc_012AA8:
	sub.w	d4,d0
	cmp.w	d4,d0
	bge.s	loc_012AA8
loc_012AAE:
	add.w	d0,d0
	move.w	$28(a6),d1
	mulu.w	$2A(a6),d1
	add.w	d1,d0
	movea.l	$C(a6),a2
	adda.w	d0,a2
	movea.l	$10(a6),a3
	move.w	$3A(a6),d4
	add.w	$32(a6),d2
	bpl.s	loc_012AD0
	add.w	d4,d2
loc_012AD0:
	cmp.w	d4,d2
	blt.s	loc_012AD6
	sub.w	d4,d2
loc_012AD6:
	move.w	d2,$32(a6)
	add.w	d3,d2
	bpl.s	loc_012AE0
	add.w	d4,d2
loc_012AE0:
	cmp.w	d4,d2
	blt.s	loc_012AE6
	sub.w	d4,d2
loc_012AE6:
	add.w	d2,d2
	add.w	$24(a6),d2
	move.w	$34(a6),d3
	mulu.w	$36(a6),d3
	add.w	d2,d3
	lsl.l	#$2,d3
	lsr.w	#$2,d3
	swap	d3
	ori.l	#$40000000,d3
	move.l	$8(a6),d5
	move.w	$40(a6),d7
	move.w	$30(a6),d6
	sub.w	$28(a6),d6
	movea.l	(g_TileQueuePtr).l,a0
	move.w	d7,d0
	lsl.w	#$3,d0
	neg.w	d0
	move.w	d0,(a0)+
	move.l	d3,(a0)+
	swap	d3
	move.w	d3,d2
	move.w	$38(a6),d4
	subq.w	#$1,d4
	and.w	d4,d3
	eor.w	d2,d3
	subq.w	#$1,d7
	bra.s	loc_012B36
loc_012B34:
	move.w	d2,(a0)+
loc_012B36:
	move.w	(a2),d0
	movem.l	(a3,d0.w),d0-d1
	add.l	d5,d0
	add.l	d5,d1
	move.l	d0,(a0)+
	add.w	$36(a6),d2
	move.w	d2,(a0)+
	move.l	d1,(a0)+
	add.w	$36(a6),d2
	and.w	d4,d2
	or.w	d3,d2
	adda.w	$2A(a6),a2
	subq.w	#$1,d6
	dbeq	d7,loc_012B34
	bne.s	loc_012B6C
	suba.w	$2C(a6),a2
	move.w	$30(a6),d6
	dbra	d7,loc_012B34
loc_012B6C:
	move.l	a0,(g_TileQueuePtr).l
	rts
; ---------------------------------------------------------------------------
; Layer_ScrollStepY - y half of Layer_ScrollStep: moves y by the pending +6 clamped to +/-16 and, on crossing a block edge, queues the block row entering at the top or bottom
; In:  a6 = layer
; Out: +2 y, +6 reduced; the row is queued like a column ($1E cell offset for the bottom edge)
; Evidence: reached by bra from Layer_ScrollStep; executed in 75 corpus runs
; ---------------------------------------------------------------------------
Layer_ScrollStepY:
	move.w	$6(a6),d1
	beq.w	loc_012BBA
	cmpi.w	#$10,d1
	bgt.s	loc_012B8E
	cmpi.w	#$FFF0,d1
	bgt.s	loc_012B92
	move.w	#$FFF0,d1
	bra.s	loc_012B92
loc_012B8E:
	move.w	#$10,d1
loc_012B92:
	sub.w	d1,$6(a6)
	move.w	$2(a6),d0
	add.w	d1,d0
	move.w	d0,$2(a6)
	tst.w	d1
	bpl.s	loc_012BBC
	not.w	d0
	andi.w	#$F,d0
	add.w	d1,d0
	bcs.w	loc_012BBA
	moveq	#-$1,d0
	moveq	#$0,d1
	moveq	#-$2,d2
	moveq	#$0,d3
	bra.s	loc_012BCC
loc_012BBA:
	rts
loc_012BBC:
	andi.w	#$F,d0
	sub.w	d1,d0
	bcc.s	loc_012BBA
	moveq	#$1,d0
	moveq	#$F,d1
	moveq	#$2,d2
	moveq	#$1E,d3
loc_012BCC:
	move.w	$30(a6),d4
	add.w	$28(a6),d0
	bpl.s	loc_012BDA
	add.w	d4,d0
	bra.s	loc_012BE0
loc_012BDA:
	cmp.w	d4,d0
	blt.s	loc_012BE0
	sub.w	d4,d0
loc_012BE0:
	move.w	d0,$28(a6)
	add.w	d1,d0
	bpl.s	loc_012BEE
loc_012BE8:
	add.w	d4,d0
	bpl.s	loc_012BF8
	bra.s	loc_012BE8
loc_012BEE:
	cmp.w	d4,d0
	blt.s	loc_012BF8
loc_012BF2:
	sub.w	d4,d0
	cmp.w	d4,d0
	bge.s	loc_012BF2
loc_012BF8:
	mulu.w	$2A(a6),d0
	move.w	$26(a6),d1
	add.w	d1,d1
	add.w	d1,d0
	movea.l	$C(a6),a2
	adda.w	d0,a2
	movea.l	$10(a6),a3
	move.w	$3C(a6),d4
	add.w	$34(a6),d2
	bpl.s	loc_012C1C
	add.w	d4,d2
	bra.s	loc_012C22
loc_012C1C:
	cmp.w	d4,d2
	blt.s	loc_012C22
	sub.w	d4,d2
loc_012C22:
	move.w	d2,$34(a6)
	add.w	d3,d2
	bpl.s	loc_012C2E
	add.w	d4,d2
	bra.s	loc_012C34
loc_012C2E:
	cmp.w	d4,d2
	blt.s	loc_012C34
	sub.w	d4,d2
loc_012C34:
	mulu.w	$36(a6),d2
	move.w	$32(a6),d3
	add.w	d3,d3
	add.w	$24(a6),d3
	add.w	d3,d2
	move.l	d2,d3
	lsl.l	#$2,d3
	lsr.w	#$2,d3
	swap	d3
	ori.l	#$40000000,d3
	move.l	$8(a6),d5
	move.w	$2E(a6),d6
	sub.w	$26(a6),d6
	move.w	$3E(a6),d7
	movea.l	(g_TileQueuePtr).l,a0
	move.w	d7,d0
	lsl.w	#$3,d0
	neg.w	d0
	move.w	d0,(a0)+
	move.l	d3,(a0)+
	swap	d3
	move.w	d3,d2
	move.w	$36(a6),d4
	subq.w	#$1,d4
	and.w	d4,d3
	eor.w	d2,d3
	subq.w	#$1,d7
	bra.s	loc_012C86
loc_012C84:
	move.w	d2,(a0)+
loc_012C86:
	move.w	(a2)+,d0
	movem.l	(a3,d0.w),d0-d1
	add.l	d5,d0
	add.l	d5,d1
	move.l	d0,(a0)+
	add.w	$36(a6),d2
	move.w	d2,(a0)+
	move.l	d1,(a0)+
	addq.w	#$4,d2
	and.w	d4,d2
	or.w	d3,d2
	subq.w	#$1,d6
	dbeq	d7,loc_012C84
	bne.s	loc_012CB6
	suba.w	$2A(a6),a2
	move.w	$2E(a6),d6
	dbra	d7,loc_012C84
loc_012CB6:
	move.l	a0,(g_TileQueuePtr).l
	rts
; ---------------------------------------------------------------------------
; Layer_FlushTileQueueIfIdle - V-int list callback: writes the tile queue to the VDP unless g_FrameBusy (the frame is still being built)
; In:  a0 = VDP control, a1 = VDP data (from the V-int)
; Out: as Layer_FlushTileQueue
; Evidence: registered with `lea Layer_FlushTileQueueIfIdle,a0 / jsr $8B7E` (V-int object list $DE9C, run by $8ADA) in 8 stage setups ($01FC98, $032F74, ...); executed in 113 corpus runs
; ---------------------------------------------------------------------------
Layer_FlushTileQueueIfIdle:
	tst.b	(g_FrameBusy).w
	beq.s	Layer_FlushTileQueue
	rts
