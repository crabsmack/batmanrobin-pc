; ---------------------------------------------------------------------------
; Text_DrawSprites - draws a string as sprites, one frame per letter from Tbl_0141B2 (8 bytes per char from $20: x advance, -, glyph slot or negative = blank, frame), keeping each glyph's art via g_TextGlyphFrames
; In:  a2 = string (ends at a byte below $20), d4/d5 = x/y of the first letter, d6 = sprite attr
; Out: d4 advanced; sprites added with Spr_AddFront
; Evidence: Spr_ChangeFrame per glyph slot ($F376 + slot) then Spr_AddFront; A-Z use slots 0-50, ! 52, ? 54, frames 270-540; only caller Text_DrawAtObject; 11 corpus runs
; ---------------------------------------------------------------------------
Text_DrawSprites:
	moveq	#$0,d0
	move.b	(a2)+,d0
	subi.w	#$20,d0
	bcc.s	loc_01413A
	rts
loc_01413A:
	lsl.w	#$3,d0
	lea.l	Tbl_0141B2(pc,d0.w),a0
	move.l	(a0)+,-(a7)
	move.w	(a0)+,d0
	bmi.s	loc_014162
	lea.l	(g_TextGlyphFrames).l,a1
	adda.w	d0,a1
	move.w	(a1),d2
	move.w	(a0)+,d1
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(a1)
	beq.s	loc_014162
	jsr	(Spr_AddFront).l
loc_014162:
	move.l	(a7)+,d0
	swap	d0
	add.w	d0,d4
	bra.s	Text_DrawSprites
