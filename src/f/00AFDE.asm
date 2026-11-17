; ---------------------------------------------------------------------------
; Scroll_DMALinePairs - DMAs 448 words from $FFE1D2 (224 interleaved plane A/B line H-scroll pairs) to the H-scroll table at VRAM $BC00
; In:  a0 = VDP control port, a2 = 4-byte command scratch (the DMA trigger is written as two words)
; Out: none
; Evidence: regs $93C0/$9401 = length $1C0 words, $95E9/$96F0/$977F = source $FFE1D2, command $7C000082 = VRAM $BC00 + DMA, increment 2 (VInt_Handler set reg 15 = 2); only from VInt_ScrollLinePairs; TAS frames 122681-128749
; ---------------------------------------------------------------------------
Scroll_DMALinePairs:
	move.l	#$93C09401,(a0)
	move.l	#$95E996F0,(a0)
	move.w	#$977F,(a0)
	move.l	#$7C000082,(a2)
	move.w	(a2),(a0)
	move.w	$2(a2),(a0)
	rts
