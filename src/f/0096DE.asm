; ---------------------------------------------------------------------------
; Scroll_LayerToScroll - converts map layer a6's tile column/row and fine offsets into a plane H-scroll word (negated pixel x) and a V-scroll word (pixel y)
; In:  a6 = layer state (+0/+2 fine x/y, low 4 bits; +$32/+$34 tile column/row)
; Out: d0 = -(((col+2)*8) | fine x), d1 = ((row+2)*8) | fine y
; Evidence: only caller Scroll_SetPlaneA (a6 = g_LayerA), which stores d0/d1 to g_HScrollA/g_VScrollA; same arithmetic as Scroll_BandsFromLayer; TAS f391-f21099, 7777 frames
; ---------------------------------------------------------------------------
Scroll_LayerToScroll:
	moveq	#$0,d0
	move.w	$32(a6),d0
	addq.w	#$2,d0
	lsl.w	#$3,d0
	dc.w	$322E,$0000	; insn: move.w $0(a6), d1
	andi.w	#$F,d1
	or.b	d1,d0
	neg.w	d0
	moveq	#$0,d1
	move.w	$34(a6),d1
	addq.w	#$2,d1
	lsl.w	#$3,d1
	move.w	$2(a6),d2
	andi.w	#$F,d2
	or.b	d2,d1
	rts
