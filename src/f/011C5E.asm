; ---------------------------------------------------------------------------
; BossMeter_DrawDigit - masks one 7-row digit glyph (Tbl_058712 glyph index, Art_058AA2 glyphs, mask at +$140) into a tile buffer
; In:  d0.w = digit, a2 = destination (4-bit pixels, 4 bytes per row)
; Out: a2 advanced 7 rows; d0, d1, a0, a1 clobbered
; Evidence: called twice by BossMeter_DrawPercent (tens and ones); 5721 TAS frames (4541-130771), only during boss fights
; ---------------------------------------------------------------------------
BossMeter_DrawDigit:
	lea.l	(Tbl_058712).l,a0
	add.w	d0,d0
	adda.w	d0,a0
	move.w	(a0)+,d0
	lsl.w	#$5,d0
	lea.l	(Art_058AA2).l,a0
	adda.w	d0,a0
	lea.l	$140(a0),a1
	move.l	(a2),d1
	and.l	(a1)+,d1
	or.l	(a0)+,d1
	move.l	d1,(a2)+
	move.l	(a2),d1
	and.l	(a1)+,d1
	or.l	(a0)+,d1
	move.l	d1,(a2)+
	move.l	(a2),d1
	and.l	(a1)+,d1
	or.l	(a0)+,d1
	move.l	d1,(a2)+
	move.l	(a2),d1
	and.l	(a1)+,d1
	or.l	(a0)+,d1
	move.l	d1,(a2)+
	move.l	(a2),d1
	and.l	(a1)+,d1
	or.l	(a0)+,d1
	move.l	d1,(a2)+
	move.l	(a2),d1
	and.l	(a1)+,d1
	or.l	(a0)+,d1
	move.l	d1,(a2)+
	move.l	(a2),d1
	and.l	(a1)+,d1
	or.l	(a0)+,d1
	move.l	d1,(a2)+
	rts
; ---------------------------------------------------------------------------
; BossMeter_QueueTileDMA - queues the meter tile buffer (g_BossMeterTiles, $100 words) for DMA to its VRAM block
; In:  none (g_BossMeterVRAM = VRAM write command)
; Out: one DMA queue entry (cost 8); nothing when DMA_Reserve fails
; Evidence: op $2CB2 call in Scr2_011B3E ($11B7A); the source registers $95 32/$96 F6/$97 7F = $FFEC64 / 2
; ---------------------------------------------------------------------------
BossMeter_QueueTileDMA:
	moveq	#$1,d0
	moveq	#$8,d1
	jsr	(DMA_Reserve).l
	bmi.s	loc_011CD6
	move.l	#$93009401,(a0)+
	move.l	#$953296F6,(a0)+
	move.w	#$977F,(a0)+
	move.l	(g_BossMeterVRAM).w,(a0)+
	move.w	a0,(g_DMAQueuePtr).w
loc_011CD6:
	rts
; ---------------------------------------------------------------------------
; BossMeter_DrawSprite - writes the meter's 4x2-tile sprite straight into the SAT buffer at the object's position
; In:  a6 = meter object (+$3E x, +$42 y), a5 = its sprite block (+$38 attr)
; Out: g_SATPtr and g_SprCount advanced
; Evidence: op $2CB2 call in Scr2_011B3E ($11B88); size $D = 4 wide x 2 tall, tile g_BossMeterTile
; ---------------------------------------------------------------------------
BossMeter_DrawSprite:
	movea.w	(g_SATPtr).w,a0
	move.w	$42(a6),(a0)+
	move.w	(g_SprCount).w,d0
	addq.b	#$1,d0
	move.w	d0,(g_SprCount).w
	ori.w	#$D00,d0
	move.w	d0,(a0)+
	move.w	(g_BossMeterTile).w,d0
	or.w	$38(a5),d0
	move.w	d0,(a0)+
	move.w	$3E(a6),(a0)+
	move.w	a0,(g_SATPtr).w
	rts
; ---------------------------------------------------------------------------
; BossMeter_Init - meter start: acquires a 16-tile VRAM block for the meter (handle g_BossMeterArt), records its DMA command and tile, and centres it between the two players' panels
; In:  a6 = meter object
; Out: d0 = 0 ok (x $3E(a6) = mean of the panels' home x + $A2), d0 = 1 when no VRAM block is free
; Evidence: first op of Scr2_011B3E (op $2D30, relative); ArtCache_Acquire with handle $ED82, d1 = 1; the VRAM command comes from the table at $80B5 indexed by the block's first tile
; ---------------------------------------------------------------------------
BossMeter_Init:
	movem.l	a2-a3,-(a7)
	lea.l	(g_BossMeterArt).l,a0
	moveq	#$1,d1
	jsr	(ArtCache_Acquire).l
	bmi.s	loc_011D46
	bne.s	loc_011D2C
	move.w	(a3)+,d0
	lea.l	(loc_0080B4+$1).l,a0
	move.l	(a0,d0.w),(g_BossMeterVRAM).w
	move.w	d0,(g_BossMeterTile).w
loc_011D2C:
	movem.l	(a7)+,a2-a3
	move.w	(g_Player1+$A).w,d0
	add.w	(g_Player2+$A).w,d0
	asr.w	#$1,d0
	addi.w	#$A2,d0
	move.w	d0,$3E(a6)
	moveq	#$0,d0
	rts
loc_011D46:
	movem.l	(a7)+,a2-a3
	moveq	#$1,d0
	rts
; ---------------------------------------------------------------------------
; BossMeter_Free - removes the meter: clears g_BossMeterObj and releases its VRAM block
; In:  none
; Out: as ArtCache_Release
; Evidence: op $2CB2 call in Scr2_011B8E (the exit script set by BossMeter_Defeated), just before Op2_Kill; executed in the TAS and scenario-ending
; ---------------------------------------------------------------------------
BossMeter_Free:
	clr.w	(g_BossMeterObj).w
	lea.l	(g_BossMeterArt).l,a0
	jmp	(ArtCache_Release).l
