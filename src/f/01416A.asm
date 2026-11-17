; ---------------------------------------------------------------------------
; Text_DrawSpritesVertical - draws a string as a vertical column of sprite letters: each glyph from Tbl_0141B2 is centred on x (minus half its width word) and the next one goes below by its height word
; In:  a2 = string (bytes >= $20, any lower byte ends it), d4/d5 = column centre x / top y, d6 = attr
; Out: a2 past the terminator; d4/d5 trashed
; Evidence: x = (2*d4 - entry+0)/2, the saved y gets + entry+2 and x is restored every letter; only caller PlayerSelect_DrawCaption $49C88 draws Tbl_014081 'PLAYER' and Tbl_01407A 'SELECT' (strings end in $0D); TAS f431-f477 = the player-select screen, whose SELECT / PLAYER run down the screen edges (screenshot f450)
; ---------------------------------------------------------------------------
Text_DrawSpritesVertical:
	moveq	#$0,d0
	move.b	(a2)+,d0
	subi.w	#$20,d0
	bcc.s	loc_014176
	rts
loc_014176:
	lsl.w	#$3,d0
	lea.l	Tbl_0141B2(pc,d0.w),a0
	move.w	d4,d0
	move.w	d5,d1
	add.w	d4,d4
	sub.w	(a0)+,d4
	asr.w	#$1,d4
	add.w	(a0)+,d1
	movem.w	d0-d1,-(a7)
	move.w	(a0)+,d0
	bmi.s	loc_0141AC
	lea.l	(g_TextGlyphFrames).l,a1
	adda.w	d0,a1
	move.w	(a1),d2
	move.w	(a0)+,d1
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(a1)
	beq.s	loc_0141AC
	jsr	(Spr_AddFront).l
loc_0141AC:
	movem.w	(a7)+,d4-d5
	bra.s	Text_DrawSpritesVertical
