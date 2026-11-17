; ---------------------------------------------------------------------------
; HUD_DrawPanelText - draws a 12-character panel message as 2 rows of 6 font tiles (ASCII - $20 through the glyph table)
; In:  a3 = text, a0 = glyph tile-index table (Tbl_0586F2), a1 = font tiles (Art_058AA2), a2 = g_HUDTileBuf
; Out: a3 past the text
; Evidence: called by HUD_StepP1Panel/HUD_StepP2Panel when the panel is in text mode (+$25 bit 7) with a pending text +$1A; texts at $10F38: ' GAME  OVER ', ' PRESS START', ' BOSS  TIME ', 'PLEASE WAIT ', and 'CREDIT nn' from HudOp_ShowCredits; P2 panel shows PRESS START in the TAS
; ---------------------------------------------------------------------------
HUD_DrawPanelText:
	moveq	#$1,d6
loc_011712:
	moveq	#$5,d7
loc_011714:
	moveq	#$0,d0
	move.b	(a3)+,d0
	subi.b	#$20,d0
	add.w	d0,d0
	move.w	(a0,d0.w),d0
	lsl.w	#$5,d0
	lea.l	(a1,d0.w),a4
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	lea.l	$20(a2),a2
	dbra	d7,loc_011714
	lea.l	-$160(a2),a2
	dbra	d6,loc_011712
	rts
