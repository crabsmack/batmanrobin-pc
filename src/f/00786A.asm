; ---------------------------------------------------------------------------
; DMA_FlushQueue_38 - computed entry into DMA_FlushQueue's unrolled copy that sends 38 queued DMA commands (2 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A
; Evidence: $786A = $785A + 2*8, reached only by the jmp at $7856; not called by name
; ---------------------------------------------------------------------------
DMA_FlushQueue_38:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_37 - computed entry into DMA_FlushQueue's unrolled copy that sends 37 queued DMA commands (3 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $7872 = $785A + 3*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_37:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_32 - computed entry into DMA_FlushQueue's unrolled copy that sends 32 queued DMA commands (8 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $789A = $785A + 8*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_32:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
