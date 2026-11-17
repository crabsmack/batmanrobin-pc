; ---------------------------------------------------------------------------
; Scroll_DMAColumnsAB - DMAs the column V-scroll of both planes from separate buffers: 20 words from $FF9000 to VSRAM 0 and 20 from $FF9028 to VSRAM 2, increment 4
; In:  a0 = VDP control port, a2 = 4-byte command scratch
; Out: VDP reg 15 back to 2
; Evidence: $8F04, $9314 = $14 words, sources $FF9000 / $FF9028 ($9514/$96C8), commands $40000090 / $40020090, $8F02; only from VInt_ScrollLinesColumns; 14 corpus runs, TAS frames 87704-108293
; ---------------------------------------------------------------------------
Scroll_DMAColumnsAB:
	move.w	#$8F04,(a0)
	move.l	#$93149400,(a0)
	move.l	#$950096C8,(a0)
	move.w	#$977F,(a0)
	move.l	#$40000090,(a2)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
	move.l	#$93149400,(a0)
	move.l	#$951496C8,(a0)
	move.w	#$977F,(a0)
	move.l	#$40020090,(a2)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
	move.w	#$8F02,(a0)
	rts
