; ---------------------------------------------------------------------------
; DMA_FlushQueue_40 - head of DMA_FlushQueue's unrolled copy: entered with no free slot, so all 40 queued DMA commands are sent (two 8-byte units here, the rest in DMA_FlushQueue_38 onward)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through DMA_FlushQueue_38 ... to the rts at $799A
; Evidence: jmp DMA_FlushQueue_40(pc,d0.w) at $7856 with d0 = free slots * 8 = 0; never executed in the corpus (the queue was never full); each unit = move.l/move.l/move.l/move.w = one 14-byte entry
; ---------------------------------------------------------------------------
DMA_FlushQueue_40:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
