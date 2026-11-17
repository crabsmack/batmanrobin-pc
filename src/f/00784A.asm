; ---------------------------------------------------------------------------
; DMA_FlushQueue - writes every queued 14-byte DMA command (VDP regs $93-$97 + the two command words) to the VDP control port
; In:  a0 = $C00004 (VDP control), g_DMAQueueFree ($9918)
; Out: a3 past the last entry; the queue itself is not reset (DMA_ResetQueue does that)
; Evidence: only caller VInt_Handler; jmp $785A(pc,free*8) enters an unrolled run of 40 move.l/move.l/move.l/move.w groups ending in rts at $799A, so it executes (40 - free) entries; entry layout matches what Spr_QueueSATDMA and $F808 write
; ---------------------------------------------------------------------------
DMA_FlushQueue:
	lea.l	(g_DMAQueue).l,a3
	move.w	(g_DMAQueueFree).w,d0
	lsl.w	#$3,d0
	jmp	DMA_FlushQueue_40(pc,d0.w)
