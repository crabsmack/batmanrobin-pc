; ---------------------------------------------------------------------------
; VDP_ClearVSRAM - zeroes the 40 VSRAM words (all vertical scroll values)
; In:  none
; Out: a0 = $C00000, d0 = 0, d7 = -1
; Evidence: VSRAM write 0 ($40000010), 5 x 16 bytes; only caller Scene_InitCommon; TAS 25 frames (scene starts)
; ---------------------------------------------------------------------------
VDP_ClearVSRAM:
	lea.l	($C00000).l,a0
	move.w	#$4,d7
	move.l	#$40000010,$4(a0)
	bra.s	VDP_ClearLoop
