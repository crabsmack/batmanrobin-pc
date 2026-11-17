; ---------------------------------------------------------------------------
; Layer_FlushTileQueue - writes every queued tile entry to the VDP through the unrolled writer Layer_WriteTiles and empties the queue
; In:  a0 = VDP control port, a1 = VDP data port
; Out: queue empty, g_TileQueuePtr rewound
; Evidence: called by Layer_DrawScreen (a0/a1 = $C00004/$C00000) and Layer_FlushTileQueueIfIdle; 92k TAS frames
; ---------------------------------------------------------------------------
Layer_FlushTileQueue:
	lea.l	(g_TileQueue).l,a3
	move.w	(a3),d0
	beq.w	Layer_WriteTilesEnd
	movea.l	(g_TileQueuePtr).w,a4
	clr.w	(a4)
	move.l	a3,(g_TileQueuePtr).w
	clr.w	(a3)+
	bra.w	Layer_WriteTilesStart
