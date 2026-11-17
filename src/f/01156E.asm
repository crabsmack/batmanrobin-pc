; ---------------------------------------------------------------------------
; HUD_DrawScore - draws the player's score (capped at 999999) as up to 6 digit tiles in the panel's top row, leading digits dropped and the rest padded by HUD_ClearTiles
; In:  a6 = player record (+0 long score), d4 = 0 (P1, left-aligned) or $60 (P2, right-aligned)
; Out: digit tiles at g_HUDTileBuf (stride $60), d7 = tiles left to clear
; Evidence: called by HUD_StepP1Panel (d4=0) and HUD_StepP2Panel (d4=$60); digits from Tbl_058712 / Art_058AA2; TAS frame 12000 shows 27630 = +0 long $6BEE
; ---------------------------------------------------------------------------
HUD_DrawScore:
	move.l	(a6),d0
	cmpi.l	#$F423F,d0
	bcs.s	loc_01157E
	move.l	#$F423F,d0
loc_01157E:
	lea.l	(Tbl_058712).l,a0
	lea.l	(Art_058AA2).l,a1
	lea.l	(g_HUDTileBuf).l,a2
	moveq	#$6,d7
	cmpi.l	#$186A0,d0
	bcc.s	loc_0115C0
	adda.w	d4,a2
	cmpi.l	#$2710,d0
	bcc.s	loc_0115CA
	adda.w	d4,a2
	cmpi.w	#$3E8,d0
	bcc.s	loc_0115D4
	adda.w	d4,a2
	cmpi.w	#$64,d0
	bcc.s	loc_0115DE
	adda.w	d4,a2
	cmpi.w	#$A,d0
	bcc.s	loc_0115E4
	adda.w	d4,a2
	bra.s	loc_0115EA
loc_0115C0:
	move.l	#$186A0,d1
	bsr.w	HUD_DrawScoreDigit
loc_0115CA:
	move.l	#$2710,d1
	bsr.w	HUD_DrawScoreDigit
loc_0115D4:
	move.l	#$3E8,d1
	bsr.w	HUD_DrawScoreDigit
loc_0115DE:
	moveq	#$64,d1
	bsr.w	HUD_DrawScoreDigit
loc_0115E4:
	moveq	#$A,d1
	bsr.w	HUD_DrawScoreDigit
loc_0115EA:
	move.w	d0,d2
	add.w	d2,d2
	bra.s	HUD_CopyScoreDigit
