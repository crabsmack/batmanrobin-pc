; ---------------------------------------------------------------------------
; VDP_ClearSAT - zeroes the sprite attribute table (VRAM $0000-$027F, 80 sprites)
; In:  none
; Out: a0 = $C00000, d0 = 0, d7 = -1
; Evidence: VRAM write 0, $28 x 16 = $280 bytes; reg 5 = 0 in Tbl_0155BC puts the SAT at VRAM 0 (Spr_QueueSATDMA targets VRAM $0000); only caller Scene_InitCommon; TAS 25 frames
; ---------------------------------------------------------------------------
VDP_ClearSAT:
	lea.l	($C00000).l,a0
	move.w	#$27,d7
	move.l	#$40000000,$4(a0)
	bra.s	VDP_ClearLoop
