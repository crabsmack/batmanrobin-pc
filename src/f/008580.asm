; ---------------------------------------------------------------------------
; ArtCache_AddBlocksInline - adds blocks to the sprite-art VRAM cache, the count read from the inline word at (a6)+
; In:  a6 -> count word
; Out: a6 past it; see ArtCache_AddBlocks
; Evidence: no static reference (reached through a pointer); ran once in the TAS (frame 4539); falls into ArtCache_AddBlocks
; ---------------------------------------------------------------------------
ArtCache_AddBlocksInline:
	move.w	(a6)+,d0
	bra.s	ArtCache_AddBlocks
