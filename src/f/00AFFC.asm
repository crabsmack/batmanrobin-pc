; ---------------------------------------------------------------------------
; Scroll_DMALines - DMAs both planes' per-line H-scroll: 224 words of g_HScrollLinesA to VRAM $BC00 and g_HScrollLinesB to $BC02, increment 4
; In:  a0 = VDP control port, a2 = 4-byte command scratch
; Out: VDP reg 15 back to 2
; Evidence: $8F04, $93E0/$9400 = $E0 words, sources $FFE1D2 / $FFE392, commands $7C000082 / $7C020082, then $8F02; called by VInt_ScrollLines/LinesColumns/LinesRaster and $24576; 112 corpus runs, TAS frames 532-130772
; ---------------------------------------------------------------------------
Scroll_DMALines:
	move.w	#$8F04,(a0)
	move.l	#$93E09400,(a0)
	move.l	#$95E996F0,(a0)
	move.w	#$977F,(a0)
	move.l	#$7C000082,(a2)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
	move.l	#$93E09400,(a0)
	move.l	#$95C996F1,(a0)
	move.w	#$977F,(a0)
	move.l	#$7C020082,(a2)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
	move.w	#$8F02,(a0)
	rts
