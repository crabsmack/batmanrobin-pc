; ---------------------------------------------------------------------------
; DMA_FlushQueue_29 - computed entry into DMA_FlushQueue's unrolled copy that sends 29 queued DMA commands (11 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A
; Evidence: $78B2 = $785A + 11*8; ran on 3 TAS frames (164-407), reached only by the jmp at $7856
; ---------------------------------------------------------------------------
DMA_FlushQueue_29:
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
; DMA_FlushQueue_16 - computed entry into DMA_FlushQueue's unrolled copy that sends 16 queued DMA commands (24 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $791A = $785A + 24*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_16:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_15 - computed entry into DMA_FlushQueue's unrolled copy that sends 15 queued DMA commands (25 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $7922 = $785A + 25*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_15:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_14 - computed entry into DMA_FlushQueue's unrolled copy that sends 14 queued DMA commands (26 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $792A = $785A + 26*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_14:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_13 - computed entry into DMA_FlushQueue's unrolled copy that sends 13 queued DMA commands (27 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $7932 = $785A + 27*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_13:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_12 - computed entry into DMA_FlushQueue's unrolled copy that sends 12 queued DMA commands (28 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $793A = $785A + 28*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_12:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_11 - computed entry into DMA_FlushQueue's unrolled copy that sends 11 queued DMA commands (29 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $7942 = $785A + 29*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_11:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_10 - computed entry into DMA_FlushQueue's unrolled copy that sends 10 queued DMA commands (30 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $794A = $785A + 30*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_10:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_9 - computed entry into DMA_FlushQueue's unrolled copy that sends 9 queued DMA commands (31 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $7952 = $785A + 31*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_9:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_8 - computed entry into DMA_FlushQueue's unrolled copy that sends 8 queued DMA commands (32 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $795A = $785A + 32*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_8:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_7 - computed entry into DMA_FlushQueue's unrolled copy that sends 7 queued DMA commands (33 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $7962 = $785A + 33*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_7:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_6 - computed entry into DMA_FlushQueue's unrolled copy that sends 6 queued DMA commands (34 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $796A = $785A + 34*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_6:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_5 - computed entry into DMA_FlushQueue's unrolled copy that sends 5 queued DMA commands (35 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $7972 = $785A + 35*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_5:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_4 - computed entry into DMA_FlushQueue's unrolled copy that sends 4 queued DMA commands (36 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $797A = $785A + 36*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_4:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_3 - computed entry into DMA_FlushQueue's unrolled copy that sends 3 queued DMA commands (37 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $7982 = $785A + 37*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_3:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_2 - computed entry into DMA_FlushQueue's unrolled copy that sends 2 queued DMA commands (38 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $798A = $785A + 38*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_2:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
; ---------------------------------------------------------------------------
; DMA_FlushQueue_1 - computed entry into DMA_FlushQueue's unrolled copy that sends 1 queued DMA command (39 slots free)
; In:  a0 = VDP control, a3 = queue start (set by DMA_FlushQueue)
; Out: falls through to the rts at $799A; a3 past the last entry
; Evidence: $7992 = $785A + 39*8, reached only by the jmp at $7856 (a runtime entry of the reference)
; ---------------------------------------------------------------------------
DMA_FlushQueue_1:
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.l	(a3)+,(a0)
	move.w	(a3)+,(a0)
	rts
