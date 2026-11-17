; ---------------------------------------------------------------------------
; ToyChain_Update - per-frame chain update: grows it, places and draws the links, refreshes their body boxes and runs their touch attack
; In:  a6 = chain object (a3/a4/a5 = its blocks), $E(a6) = chain block
; Out: a2-a6 preserved
; Evidence: calls ToyChain_Grow, ToyChain_PlaceAndDrawLinks, ToyChain_PlaceLinkBoxes, ToyChain_LinkTouchAttack with a2 = $E(a6); Op2_CallNativeRel from the chain loops $037A1E/$037BE8/$037E14/$03806C; TAS f79928-f112311 (6553 frames)
; ---------------------------------------------------------------------------
ToyChain_Update:
	movem.l	a2-a6,-(a7)
	movea.w	$E(a6),a2
	bsr.w	ToyChain_Grow
	bsr.w	ToyChain_PlaceAndDrawLinks
	bsr.w	ToyChain_PlaceLinkBoxes
	bsr.w	ToyChain_LinkTouchAttack
	movem.l	(a7)+,a2-a6
	rts
