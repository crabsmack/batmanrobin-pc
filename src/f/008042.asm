; ---------------------------------------------------------------------------
; Spr_QueueArtDMA - queues one DMA per art chunk from ROM into the frame's allocated 16-tile VRAM blocks and charges the DMA budget
; In:  a4 = art header (word cost, word chunk count n, n chunk codes = 4 * tiles, then the tile data), a3 = destination block list (tile indexes, from ArtCache_Alloc)
; Out: n entries in g_DMAQueue; g_DMAQueueFree -= n, g_DMABudgetLeft -= cost + n
; Evidence: callers Spr_SwapFrameArt and $6776, both right after ArtCache_Alloc; length regs come from $80D0 + code ($80D4 = $93109400 = 16 words = 1 tile), the VRAM command from $80B5 + tile ($8114 = $4BE00080 = VRAM write DMA at tile $5F)
; ---------------------------------------------------------------------------
Spr_QueueArtDMA:
	move.w	(a4)+,d0
	moveq	#$0,d7
	move.w	(a4)+,d7
	sub.w	d7,(g_DMAQueueFree).w
	add.w	d7,d0
	sub.w	d0,(g_DMABudgetLeft).w
	movea.w	(g_DMAQueuePtr).w,a0
	lea.l	(Tbl_0080D4-$4).l,a1
	lea.l	(loc_0080B4+$1).l,a2
	move.l	a4,d3
	lsr.l	#$1,d3
	add.l	d7,d3
	move.w	d3,d4
	lsr.w	#$8,d4
	andi.l	#$FF00FF,d3
	subq.w	#$1,d7
	beq.s	loc_0080B4
	move.l	d3,d5
	swap	d5
	ori.w	#$9500,d3
	ori.w	#$9600,d4
	ori.w	#$9700,d5
	moveq	#$0,d2
	bra.s	loc_008098
loc_00808A:
	add.b	d0,d0
	add.b	d0,d3
	addx.b	d2,d4
	addx.b	d2,d5
	add.b	d0,d3
	addx.b	d2,d4
	addx.b	d2,d5
loc_008098:
	move.w	(a4)+,d0
	move.l	(a1,d0.w),(a0)+
	move.w	d3,(a0)+
	move.w	d4,(a0)+
	move.w	d5,(a0)+
	move.w	(a3)+,d1
	move.l	(a2,d1.w),(a0)+
	dbra	d7,loc_00808A
	move.w	a0,(g_DMAQueuePtr).w
	rts
loc_0080B4:
	ori.l	#$97009500,d3
	ori.w	#$9600,d4
	move.w	(a4)+,d0
	move.l	(a1,d0.w),(a0)+
	move.l	d3,(a0)+
	move.w	d4,(a0)+
	move.w	(a3),d1
	move.l	(a2,d1.w),(a0)+
	move.w	a0,(g_DMAQueuePtr).w
	rts
