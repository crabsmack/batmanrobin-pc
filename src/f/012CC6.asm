; ---------------------------------------------------------------------------
; Layer_ResetTileQueue - empties the tile queue: terminates it at the current write pointer and rewinds g_TileQueuePtr to g_TileQueue
; In:  none
; Out: g_TileQueue = 0 (end), g_TileQueuePtr = g_TileQueue
; Evidence: called by the scene init $9AAC; once per scene in every run (TAS 25 times)
; ---------------------------------------------------------------------------
Layer_ResetTileQueue:
	lea.l	(g_TileQueue).l,a3
	movea.l	(g_TileQueuePtr).w,a4
	clr.w	(a4)
	move.l	a3,(g_TileQueuePtr).w
	clr.w	(a3)
	rts
