; ---------------------------------------------------------------------------
; ArtCache_AddBlocks - appends d0 more 16-tile VRAM blocks, continuing from g_ArtNextBlockTile, to the free block ring
; In:  d0 = block count
; Out: g_ArtBlocksFree/Spare += d0, g_ArtBlockRingWr and g_ArtNextBlockTile advanced
; Evidence: same layout ArtCache_Init builds (tile +$10 per block, ring pointer low byte +2); entered from ArtCache_AddBlocksInline
; ---------------------------------------------------------------------------
ArtCache_AddBlocks:
	add.b	d0,(g_ArtBlocksFree).w
	add.b	d0,(g_ArtBlocksSpare).w
	move.w	(g_ArtBlockRingWr).w,d1
	move.w	(g_ArtNextBlockTile).w,d2
	bra.s	loc_0085A2
loc_008598:
	movea.w	d1,a0
	move.w	d2,(a0)
	addq.b	#$2,d1
	addi.w	#$10,d2
loc_0085A2:
	dbra	d0,loc_008598
	move.w	d1,(g_ArtBlockRingWr).w
	move.w	d2,(g_ArtNextBlockTile).w
	rts
; ArtCache_Acquire - takes a reference to the art behind handle a0, allocating d1 blocks for it when it is not resident
; In:  a0 = handle address, d1 = blocks needed
; Out: d0 = 0 allocated (a3 = block list; caller loads the tiles), 1 = resident and already in use, 2 = revived from the cache, -1 = not enough free blocks, -2 = no free descriptor
; Evidence: only static caller $11D04 (handle $ED82, d1 = 1), which on d0 = 0 builds its own DMA command from the $80B5 table and releases with ArtCache_Release at $11D4E
ArtCache_Acquire:
	move.w	(a0),d0
	bne.s	loc_0085DC
	sub.b	d1,(g_ArtBlocksFree).w
	bcs.s	loc_0085C8
	subq.b	#$1,(g_ArtDescsFree).w
	bcs.s	loc_0085D0
	bsr.w	ArtCache_Alloc
	moveq	#$0,d0
	rts
loc_0085C8:
	add.b	d1,(g_ArtBlocksFree).w
	moveq	#-$1,d0
	rts
loc_0085D0:
	clr.b	(g_ArtDescsFree).w
	add.b	d1,(g_ArtBlocksFree).w
	moveq	#-$2,d0
	rts
loc_0085DC:
	movea.w	d0,a0
	subq.b	#$1,(a0)+
	bcc.s	loc_0085F0
	move.b	(a0),d0
	sub.b	d0,(g_ArtBlocksFree).w
	subq.b	#$1,(g_ArtDescsFree).w
	moveq	#$2,d0
	rts
loc_0085F0:
	bmi.s	loc_0085F4
	addq.b	#$1,(a0)
loc_0085F4:
	moveq	#$1,d0
	rts
