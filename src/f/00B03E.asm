; ---------------------------------------------------------------------------
; Scroll_DMALinesA - DMAs plane A's per-line H-scroll only: 224 words of g_HScrollLinesA to VRAM $BC00, increment 4
; In:  a0 = VDP control port, a2 = 4-byte command scratch
; Out: VDP reg 15 back to 2
; Evidence: first half of Scroll_DMALines' register writes; from VInt_ScrollLinesARaster and VInt_ScrollLinesA; 9 corpus runs, TAS frames 431-116145
; ---------------------------------------------------------------------------
Scroll_DMALinesA:
	move.w	#$8F04,(a0)
	move.l	#$93E09400,(a0)
	move.l	#$95E996F0,(a0)
	move.w	#$977F,(a0)
	move.l	#$7C000082,(a2)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
	move.w	#$8F02,(a0)
	rts
