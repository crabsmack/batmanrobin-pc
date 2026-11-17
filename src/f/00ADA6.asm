; ---------------------------------------------------------------------------
; VDP_ClearCRAM - sets all 64 CRAM colours to black
; In:  none
; Out: a0 = $C00000, d0 = 0, d7 = -1
; Evidence: CRAM write 0 ($C0000000), 8 x 16 bytes; callers Main_Start and Scene_InitCommon (when g_FadeLevel < $E); TAS 26 frames
; ---------------------------------------------------------------------------
VDP_ClearCRAM:
	lea.l	($C00000).l,a0
	move.w	#$7,d7
	move.l	#$C0000000,$4(a0)
	bra.s	VDP_ClearLoop
