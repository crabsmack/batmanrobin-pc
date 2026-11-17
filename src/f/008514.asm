; ---------------------------------------------------------------------------
; ArtCache_Init - sets up the sprite-art VRAM cache: d1 16-tile blocks from tile $5F, 50 descriptors, empty release ring, all art handles cleared
; In:  d1 = number of blocks
; Out: $FF8000-$FF8CFF cleared (handles); block ring $8D00 = tiles $5F, $6F, ...; descriptor ring $8E00 = 50 x $1E-byte descriptors at $A774; release ring $8F00 empty; g_ArtBlocksFree/Spare = d1, g_ArtDescsFree/Spare = 50, g_ArtNextBlockTile = next tile
; Evidence: only caller scene init $9AAC with d1 = a word of the scene script (a6)+; ran on 25 TAS frames (scene loads)
; ---------------------------------------------------------------------------
ArtCache_Init:
	lea.l	(g_ArtHandles).w,a0
	lea.l	(g_ArtBlockRing).w,a1
	lea.l	(g_ArtDescRing).w,a2
	lea.l	(g_ArtRelRing).w,a3
	move.w	#$680,d0
	moveq	#$32,d2
	move.b	d1,(g_ArtBlocksFree).w
	move.b	d1,(g_ArtBlocksSpare).w
	move.b	d2,(g_ArtDescsFree).w
	move.b	d2,(g_ArtDescsSpare).w
	move.w	a3,(g_ArtRelRingRd).w
	move.w	a3,(g_ArtRelRingWr).w
	subq.w	#$1,d0
loc_008544:
	clr.w	(a0)+
	dbra	d0,loc_008544
	move.w	a1,(g_ArtBlockRingRd).w
	move.w	#$5F,d0
	subq.w	#$1,d1
loc_008554:
	move.w	d0,(a1)+
	addi.w	#$10,d0
	dbra	d1,loc_008554
	move.w	a1,(g_ArtBlockRingWr).w
	move.w	d0,(g_ArtNextBlockTile).w
	move.w	a2,(g_ArtDescRingRd).w
	lea.l	(g_ArtDescs).w,a0
	subq.w	#$1,d2
loc_008570:
	move.w	a0,(a2)+
	lea.l	$1E(a0),a0
	dbra	d2,loc_008570
	move.w	a2,(g_ArtDescRingWr).w
	rts
