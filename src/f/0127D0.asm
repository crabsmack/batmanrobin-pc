; ---------------------------------------------------------------------------
; Layer_Init - initialises a map layer from a level header of 8 self-relative pointers: block map size and data, block tiles, block table, collision data, animated tiles, plus the scene's plane geometry
; In:  a6 = layer struct, a0 = level header, d0 = tile word added to every block cell (+8/+$A)
; Out: +0..+6 cleared, +$C map, +$10 block tiles (2 longs per block), +$14 block table, +$18 collision data, +$1C/+$20/+$42 animated tiles, +$26/+$28 block column/row = size-1, +$2A row bytes, +$2C map bytes, +$2E/+$30 width/height in blocks, +$32/+$34 plane cell column/row, +$36-$40 from g_Plane*/g_ScreenBlocks*; a1/a2 = the header's first two pointers
; Evidence: called by the scene-script op $7202 (layer a1 = g_LayerA from $71CC/$71DA with +$24 = plane VRAM base); runs at every scene start (TAS 390-90423, 9 times); Coll_MoveBlockMap reads the +$C/+$14/+$18/+$2A/+$2E/+$30 fields it sets
; ---------------------------------------------------------------------------
Layer_Init:
	clr.w	(a6)
	clr.w	$2(a6)
	clr.w	$4(a6)
	clr.w	$6(a6)
	move.w	d0,$8(a6)
	move.w	d0,$A(a6)
	move.w	d0,d6
	andi.w	#$3FFF,d0
	lsl.w	#$5,d0
	move.w	d0,$4A(a6)
	move.w	(g_PlaneRowBytes).w,$36(a6)
	move.w	(g_PlaneBytes).w,$38(a6)
	move.w	(g_ScreenBlocksX).w,$3E(a6)
	move.w	(g_ScreenBlocksY).w,$40(a6)
	move.w	(g_PlaneCellsX).w,d1
	move.w	(g_PlaneCellsY).w,d2
	move.w	d1,$3A(a6)
	move.w	d2,$3C(a6)
	subq.w	#$2,d1
	subq.w	#$2,d2
	move.w	d1,$32(a6)
	move.w	d2,$34(a6)
	movea.l	a0,a1
	adda.l	(a0)+,a1
	movea.l	a0,a2
	adda.l	(a0)+,a2
	movea.l	a0,a3
	adda.l	(a0)+,a3
	movea.l	a0,a4
	adda.l	(a0)+,a4
	move.l	a4,$10(a6)
	movea.l	a0,a4
	adda.l	(a0)+,a4
	move.l	a4,$14(a6)
	movea.l	a0,a4
	adda.l	(a0)+,a4
	move.l	a4,$18(a6)
	movea.l	a0,a4
	adda.l	(a0)+,a4
	move.w	(a4)+,$42(a6)
	move.l	a4,$1C(a6)
	movea.l	a0,a4
	adda.l	(a0)+,a4
	move.l	a4,$20(a6)
	move.w	(a3)+,d1
	move.w	(a3)+,d2
	move.l	a3,$C(a6)
	move.w	d1,$26(a6)
	subq.w	#$1,$26(a6)
	move.w	d2,$28(a6)
	subq.w	#$1,$28(a6)
	move.w	d1,$2E(a6)
	move.w	d2,$30(a6)
	add.w	d1,d1
	move.w	d1,$2A(a6)
	mulu.w	d1,d2
	move.w	d2,$2C(a6)
	clr.w	$44(a6)
	clr.w	$46(a6)
	clr.w	$48(a6)
	rts
