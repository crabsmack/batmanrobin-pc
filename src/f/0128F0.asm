; ---------------------------------------------------------------------------
; Layer_MoveY - moves a layer's y by d0 pixels without drawing, stepping its block row +$28 (wrapping at the map height) by the block boundaries crossed
; In:  a6 = layer, d0 = signed pixels
; Out: +2 y and +$28 updated
; Evidence: called by the scene-script ops $739E/$73DE and $032F14 (d0 = $1910 for the vertical zeppelin stage, TAS f22200); runs at scene starts (TAS 390-67239)
; ---------------------------------------------------------------------------
Layer_MoveY:
	move.w	$2(a6),d1
	add.w	d0,d1
	move.w	d1,$2(a6)
	move.w	d0,d2
	bpl.s	loc_01291A
	neg.w	d2
	lsr.w	#$4,d2
	neg.w	d2
	neg.w	d0
	andi.w	#$F,d0
	not.w	d1
	andi.w	#$F,d1
	sub.w	d0,d1
	bcc.w	loc_01292A
	subq.w	#$1,d2
	bra.s	loc_01292A
loc_01291A:
	lsr.w	#$4,d2
	andi.w	#$F,d0
	andi.w	#$F,d1
	sub.w	d0,d1
	bcc.s	loc_01292A
	addq.w	#$1,d2
loc_01292A:
	move.w	$30(a6),d1
	add.w	$28(a6),d2
	bpl.s	loc_012936
	add.w	d1,d2
loc_012936:
	cmp.w	d1,d2
	blt.s	loc_01293C
	sub.w	d1,d2
loc_01293C:
	move.w	d2,$28(a6)
	rts
; ---------------------------------------------------------------------------
; Layer_DrawScreen - draws a full screen of the layer straight to the VDP: g_ScreenBlocksX block columns (each queued by Layer_QueueColumn and flushed at once), then restores the column counters
; In:  a6 = layer
; Out: plane cells written through $C00004/$C00000; +$26/+$32 unchanged
; Evidence: pointed to from 8 scene setups ($00740E, $01FC92, $032F6E, ...); runs once per scene start in 15 corpus runs (idle, options, robin, TAS)
; ---------------------------------------------------------------------------
Layer_DrawScreen:
	move.w	$26(a6),-(a7)
	move.w	$32(a6),-(a7)
	move.w	$3E(a6),d7
	subq.w	#$1,d7
loc_012950:
	move.w	d7,-(a7)
	moveq	#$1,d0
	moveq	#$0,d1
	moveq	#$2,d2
	moveq	#$0,d3
	bsr.w	Layer_QueueColumn
	lea.l	($C00004).l,a0
	lea.l	-$4(a0),a1
	bsr.w	Layer_FlushTileQueue
	move.w	(a7)+,d7
	dbra	d7,loc_012950
	move.w	(a7)+,$32(a6)
	move.w	(a7)+,$26(a6)
	rts
