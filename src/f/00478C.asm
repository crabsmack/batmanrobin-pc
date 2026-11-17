; ---------------------------------------------------------------------------
; Op2_DrawSprBack - draw-list op: draws one frame at the object's position behind every sorted sprite (Spr_AddBack)
; In:  a2 = draw list: word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: Spr_ChangeFrame / bsr Spr_AddBack; 57 runs; $284FC `478C 3A20 44F2`
; ---------------------------------------------------------------------------
Op2_DrawSprBack:
	move.w	$38(a5),d6
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddBack
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprBackAttr - draw-list op: Op2_DrawSprBack with the attribute byte change (and-mask, xor)
; In:  a2 = draw list: byte and-mask, byte xor, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: and.b/eor.b on $38(a5), Spr_AddBack; 42 runs; $2FF88 `47AA 9800 4088`
; ---------------------------------------------------------------------------
Op2_DrawSprBackAttr:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	move.b	$38(a5),d6
	and.b	(a2)+,d6
	move.b	(a2)+,d0
	eor.b	d0,d6
	lsl.w	#$8,d6
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddBack
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprBackOff - draw-list op: Op2_DrawSprBack at a mirrored byte offset from the object
; In:  a2 = draw list: byte dx, byte dy, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: byte offsets negated by flip bits 3/4, Spr_AddBack; 13 runs; $2F7E0 `47D0 0000 4312`
; ---------------------------------------------------------------------------
Op2_DrawSprBackOff:
	move.w	$38(a5),d6
	move.b	(a2)+,d4
	move.b	(a2)+,d5
	ext.w	d4
	ext.w	d5
	btst	#$3,$38(a5)
	beq.s	loc_0047E6
	neg.w	d4
loc_0047E6:
	btst	#$4,$38(a5)
	beq.s	loc_0047F0
	neg.w	d5
loc_0047F0:
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddBack
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprBackAttrOff - draw-list op: Op2_DrawSprBack with the attribute byte change and a mirrored byte offset
; In:  a2 = draw list: byte and-mask, byte xor, byte dx, byte dy, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: Attr then Off code, Spr_AddBack; 11 runs; $2F858 `480A E810 00FF 4330`
; ---------------------------------------------------------------------------
Op2_DrawSprBackAttrOff:
	move.b	$38(a5),d6
	and.b	(a2)+,d6
	move.b	(a2)+,d0
	eor.b	d0,d6
	lsl.w	#$8,d6
	move.b	(a2)+,d4
	move.b	(a2)+,d5
	ext.w	d4
	ext.w	d5
	btst	#$3,$38(a5)
	beq.s	loc_004828
	neg.w	d4
loc_004828:
	btst	#$4,$38(a5)
	beq.s	loc_004832
	neg.w	d5
loc_004832:
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddBack
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprAtDrawn - draw-list op: Op2_DrawSpr at the object's drawn position $4E/$50(a6) instead of x/y
; In:  a2 = draw list: word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: move.w $4E(a6),d4 / move.w $50(a6),d5, Spr_AddSorted; 16 runs; $19B34 `484C 31E2`
; ---------------------------------------------------------------------------
Op2_DrawSprAtDrawn:
	move.w	$38(a5),d6
	move.w	$3A(a5),d7
	move.w	$4E(a6),d4
	move.w	$50(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddSorted
	movea.w	(a2)+,a0
	jmp	(a0)
