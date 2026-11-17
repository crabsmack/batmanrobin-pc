; ---------------------------------------------------------------------------
; VDP_ClearHScroll - zeroes VRAM $BC00-$C3FF: the H-scroll table (and the tile $5FC that VDP_ClearPlanes fills the planes with)
; In:  none
; Out: a0 = $C00000, d0 = 0, d7 = -1
; Evidence: VRAM write $BC00 ($7C000002), $80 x 16 = 2 KB; reg 13 = $2F (H-scroll at $BC00) in Tbl_0155BC, the V-int's line buffers go to $BC00/$BC02 (NOTES 'Parallax line scroll'); only caller Scene_InitCommon; TAS 25 frames
; ---------------------------------------------------------------------------
VDP_ClearHScroll:
	lea.l	($C00000).l,a0
	move.w	#$7F,d7
	move.l	#$7C000002,$4(a0)
	bra.s	VDP_ClearLoop
