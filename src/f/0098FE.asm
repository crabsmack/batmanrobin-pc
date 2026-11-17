; ---------------------------------------------------------------------------
; Scroll_BandsFromLayer - takes a map layer's pixel position as the band origin (x as 16.8, y) and falls into Scroll_FillBands
; In:  a6 = layer (+0/+2 fine x/y, +$32/+$34 tile column/row), a0 = line buffer, a1 = band table
; Out: as Scroll_FillBands; d3 = layer pixel y
; Evidence: only caller ScrollCmd_LayerA; d2 = (((col+2)*8) | fine x) << 8, d3 = ((row+2)*8) | fine y (the Scroll_LayerToScroll arithmetic); no rts, runs into $9928
; ---------------------------------------------------------------------------
Scroll_BandsFromLayer:
	moveq	#$0,d2
	move.w	$32(a6),d2
	addq.w	#$2,d2
	asl.w	#$3,d2
	dc.w	$3E2E,$0000	; insn: move.w $0(a6), d7
	andi.w	#$F,d7
	or.b	d7,d2
	asl.l	#$8,d2
	moveq	#$0,d3
	move.w	$34(a6),d3
	addq.w	#$2,d3
	asl.w	#$3,d3
	move.w	$2(a6),d7
	andi.w	#$F,d7
	or.b	d7,d3
