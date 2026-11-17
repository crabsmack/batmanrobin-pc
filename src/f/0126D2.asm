; ---------------------------------------------------------------------------
; Layer_SetAnimTiles - sets a layer's animated tiles: VRAM base from the tile word d0 and the frame list / art pointers from a two-pointer header
; In:  a6 = layer (g_LayerA), a0 = header (2 longs relative to themselves: frame list = word count + 6-byte entries, tile art), d0 = tile word (index in the low 14 bits)
; Out: +$4A VRAM byte base, +$42 frame count, +$1C frame list, +$20 art
; Evidence: called from the stage scene $047BD0 (a0 = Tbl_0B29C6, d0 = $3C6, then +$48 = 4); ran once in the TAS (128791) and scenario-level-skip (14046); Layer_Init does the same from its header's last two pointers
; ---------------------------------------------------------------------------
Layer_SetAnimTiles:
	andi.w	#$3FFF,d0
	lsl.w	#$5,d0
	move.w	d0,$4A(a6)
	movea.l	a0,a1
	adda.l	(a0)+,a1
	move.w	(a1)+,$42(a6)
	move.l	a1,$1C(a6)
	movea.l	a0,a1
	adda.l	(a0)+,a1
	move.l	a1,$20(a6)
	rts
; ---------------------------------------------------------------------------
; Layer_AnimateTiles - per-frame tile animation of a layer: every +$48+1 frames steps to the next of +$42 frames and queues a DMA of that frame's tiles
; In:  a6 = layer (+$1C list of 6-byte entries: art offset, VRAM offset, byte length; +$20 art; +$4A VRAM base; +$44 frame, +$46 timer)
; Out: one DMA queue entry (cost length/32+1) when a frame changes; nothing when +$42 = 0 or DMA_Reserve fails
; Evidence: called by the stage loops $01FECC, $033398 and $047BD0 every frame with a6 = g_LayerA; executed in 25 corpus runs (TAS, scenario-ending, level-skip)
; ---------------------------------------------------------------------------
Layer_AnimateTiles:
	move.w	$42(a6),d1
	beq.w	loc_01278E
	subq.w	#$1,$46(a6)
	bpl.w	loc_01278E
	move.w	$48(a6),$46(a6)
	move.w	$44(a6),d0
	addq.w	#$1,d0
	cmp.w	d1,d0
	blt.s	loc_012714
	moveq	#$0,d0
loc_012714:
	move.w	d0,$44(a6)
	movea.l	$1C(a6),a5
	add.w	d0,d0
	adda.w	d0,a5
	add.w	d0,d0
	adda.w	d0,a5
	movea.l	$20(a6),a4
	adda.w	(a5)+,a4
	move.w	$4A(a6),d2
	add.w	(a5)+,d2
	move.w	(a5)+,d1
	move.w	d1,d3
	lsr.w	#$5,d1
	addq.w	#$1,d1
	moveq	#$1,d0
	jsr	(DMA_Reserve).l
	bne.s	loc_01278E
	move.l	a4,d4
	lsr.l	#$1,d4
	move.w	d4,d5
	andi.l	#$7F00FF,d4
	ori.l	#$97009500,d4
	lsr.w	#$8,d5
	ori.w	#$9600,d5
	move.l	d4,(a0)+
	move.w	d5,(a0)+
	andi.l	#$FFFF,d3
	lsl.l	#$7,d3
	lsr.w	#$8,d3
	andi.l	#$FF00FF,d3
	ori.l	#$94009300,d3
	move.l	d3,(a0)+
	andi.l	#$FFFF,d2
	lsl.l	#$2,d2
	lsr.w	#$2,d2
	swap	d2
	ori.l	#$40000080,d2
	move.l	d2,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
loc_01278E:
	rts
; ---------------------------------------------------------------------------
; Camera_TrackPlayersXLayerA - Camera_TrackPlayersX, then sets plane A's pending scroll (+4/+6 of g_LayerA) to the view position minus the layer's position
; In:  none
; Out: g_LayerA +4 = g_ViewX - x, +6 = g_ViewY - y (consumed by Layer_ScrollStep)
; Evidence: tail of the gameplay frame variant $009BD4 (jmp); executed in 49 corpus runs (TAS, level-skip)
; ---------------------------------------------------------------------------
Camera_TrackPlayersXLayerA:
	jsr	(Camera_TrackPlayersX).l
	lea.l	(g_LayerA).l,a6
	move.w	(g_ViewX).l,d0
	sub.w	(a6),d0
	move.w	d0,$4(a6)
	move.w	(g_ViewY).l,d0
	sub.w	$2(a6),d0
	move.w	d0,$6(a6)
	rts
