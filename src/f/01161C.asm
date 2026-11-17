; ---------------------------------------------------------------------------
; HUD_ClearTiles - clears d7 more tiles of the panel's top row (one per $60-byte column), padding after the score digits
; In:  a2 = buffer position, d7 = tiles to clear
; Out: a2 advanced
; Evidence: called by HUD_StepP1Panel/HUD_StepP2Panel right after HUD_DrawScore, with HUD_DrawScore's d7
; ---------------------------------------------------------------------------
HUD_ClearTiles:
	bra.s	loc_011632
loc_01161E:
	clr.l	(a2)+
	clr.l	(a2)+
	clr.l	(a2)+
	clr.l	(a2)+
	clr.l	(a2)+
	clr.l	(a2)+
	clr.l	(a2)+
	clr.l	(a2)+
	lea.l	$40(a2),a2
loc_011632:
	dbra	d7,loc_01161E
	rts
