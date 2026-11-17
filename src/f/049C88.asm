; ---------------------------------------------------------------------------
; PlayerSelect_DrawCaption - draws the player-select captions as vertical sprite-letter columns: PLAYER at the object's x/y (left edge, moving up) and SELECT at x + $10E, y = $57 - y (right edge, moving the opposite way)
; In:  a6 = the caption object
; Out: a1-a6 kept (movem); sprites added
; Evidence: Text_DrawSpritesVertical with Tbl_014081 ('PLAYER\r' at $14081) and Tbl_01407A ('SELECT\r' at $1407A), ROM bytes; png x-robin f540: PLAYER down the left edge, SELECT down the right; TAS f431-f476
; ---------------------------------------------------------------------------
PlayerSelect_DrawCaption:
	movem.l	a1-a6,-(a7)
	lea.l	(Tbl_014081).l,a2
	move.w	#$0,d6
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	jsr	(Text_DrawSpritesVertical).l
	lea.l	(Tbl_01407A).l,a2
	move.w	#$0,d6
	move.w	$3E(a6),d4
	addi.w	#$10E,d4
	move.w	#$57,d5
	sub.w	$42(a6),d5
	jsr	(Text_DrawSpritesVertical).l
	movem.l	(a7)+,a1-a6
	rts
