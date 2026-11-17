; ---------------------------------------------------------------------------
; HUD_DrawScoreDigit - emits one decimal digit of d0 for the place value d1 (repeated subtraction) and copies its tile into the HUD buffer
; In:  d0 = remaining value, d1 = place value, a0 = digit tile-index table, a1 = tile art, a2 = buffer position, d7 = tiles left
; Out: d0 = remainder, a2 advanced one column ($60), d7 - 1
; Evidence: HUD_DrawScore calls it for 100000..10 and enters its copy tail at $115FA for the units digit
; ---------------------------------------------------------------------------
HUD_DrawScoreDigit:
	moveq	#-$2,d2
loc_0115F2:
	addq.w	#$2,d2
	sub.l	d1,d0
	bcc.s	loc_0115F2
	add.l	d1,d0
; ---------------------------------------------------------------------------
; HUD_CopyScoreDigit - copies one digit tile (index d2 into the digit table) into the HUD buffer and steps one column
; In:  d2 = digit x 2, a0 = digit tile-index table, a1 = tile art, a2 = buffer position, d7 = tiles left
; Out: a2 advanced one column ($60), d7 - 1, d3/a3 clobbered
; Evidence: the copy tail of HUD_DrawScoreDigit; HUD_DrawScore branches here for the units digit (bra.s at $115EE)
; ---------------------------------------------------------------------------
HUD_CopyScoreDigit:
	move.w	(a0,d2.w),d3
	lsl.w	#$5,d3
	lea.l	(a1,d3.w),a3
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	move.l	(a3)+,(a2)+
	lea.l	$40(a2),a2
	subq.w	#$1,d7
	rts
