; ---------------------------------------------------------------------------
; Scroll_DMAColumns - DMAs 40 words of 2-cell column V-scroll (20 interleaved plane A/B pairs) from $FF9000 to VSRAM 0
; In:  a0 = VDP control port, a2 = 4-byte command scratch
; Out: none
; Evidence: $9328/$9400 = $28 words, $9500/$96C8/$977F = source $FF9000, command $40000090 = VSRAM write 0 + DMA, increment 2; called by the H-int jmp-stub setups $244A8 and $24576 (stage 2); 20 corpus runs, TAS frames 21158-35416
; ---------------------------------------------------------------------------
Scroll_DMAColumns:
	move.l	#$93289400,(a0)
	move.l	#$950096C8,(a0)
	move.w	#$977F,(a0)
	move.l	#$40000090,(a2)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
	rts
