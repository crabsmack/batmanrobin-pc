; ---------------------------------------------------------------------------
; VDP_WhiteCRAM - sets all 64 CRAM colours to white ($0EEE)
; In:  none
; Out: a0 = $C00000, d0 = $0EEE0EEE, d7 = -1
; Evidence: CRAM write 0, 8 x 16 bytes of $0EEE; only caller Scene_InitCommon when g_FadeLevel >= $E (a scene entered after a fade to white); 6 corpus runs
; ---------------------------------------------------------------------------
VDP_WhiteCRAM:
	lea.l	($C00000).l,a0
	move.w	#$7,d7
	move.l	#$C0000000,$4(a0)
	move.l	#$EEE0EEE,d0
	bra.s	VDP_FillLoop
