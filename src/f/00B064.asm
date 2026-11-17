; ---------------------------------------------------------------------------
; Scroll_DMALinesB - DMAs plane B's per-line H-scroll only: 224 words of g_HScrollLinesB to VRAM $BC02, increment 4
; In:  a0 = VDP control port, a2 = 4-byte command scratch
; Out: VDP reg 15 back to 2
; Evidence: second half of Scroll_DMALines' register writes (source $FFE392, $7C020082); from VInt_ScrollLinesBRaster; 16 corpus runs, TAS frames 35433-46289
; ---------------------------------------------------------------------------
Scroll_DMALinesB:
	move.w	#$8F04,(a0)
	move.l	#$93E09400,(a0)
	move.l	#$95C996F1,(a0)
	move.w	#$977F,(a0)
	move.l	#$7C020082,(a2)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
	move.w	#$8F02,(a0)
	rts
