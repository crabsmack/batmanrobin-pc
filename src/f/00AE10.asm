; ---------------------------------------------------------------------------
; VDP_ClearPlanes - fills planes A and B (VRAM $C000-$FFFF) with tile $5FC, a blank tile
; In:  none
; Out: a0 = $C00000, d0 = $05FC05FC, d7 = -1
; Evidence: VRAM write $C000 ($40000003), $400 x 16 = 16 KB of name word $05FC; regs 2 = $30 (plane A $C000) and 4 = $07 (plane B $E000), 64x64 cells, in Tbl_0155BC; tile $5FC lies at VRAM $BF80, zeroed by VDP_ClearHScroll just before (blank inferred from that); only caller Scene_InitCommon; TAS 25 frames
; ---------------------------------------------------------------------------
VDP_ClearPlanes:
	lea.l	($C00000).l,a0
	move.w	#$3FF,d7
	move.l	#$40000003,$4(a0)
	move.l	#$5FC05FC,d0
	bra.w	VDP_FillLoop
