; ---------------------------------------------------------------------------
; Op2_DrawSprAtDrawnDepth - draw-list op: Op2_DrawSprAtDrawn with a depth offset
; In:  a2 = draw list: word depth offset, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: $4E/$50(a6), add.w (a2)+,d7; 2 runs; $19C74 `4898 FFF6 31D8`
; ---------------------------------------------------------------------------
Op2_DrawSprAtDrawnDepth:
	move.w	$38(a5),d6
	move.w	$4E(a6),d4
	move.w	$50(a6),d5
	move.w	$3A(a5),d7
	add.w	(a2)+,d7
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddSorted
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprAtDrawnAttrDepth - draw-list op: Op2_DrawSprAtDrawn with the attribute byte change and a depth offset
; In:  a2 = draw list: byte and-mask, byte xor, word depth offset, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: $4E/$50(a6), and.b/eor.b, add.w (a2)+,d7; 2 runs; $19C7E `48BC F000 FFF6 FD30`
; ---------------------------------------------------------------------------
Op2_DrawSprAtDrawnAttrDepth:
	move.w	$4E(a6),d4
	move.w	$50(a6),d5
	move.b	$38(a5),d6
	and.b	(a2)+,d6
	move.b	(a2)+,d0
	eor.b	d0,d6
	lsl.w	#$8,d6
	move.w	$3A(a5),d7
	add.w	(a2)+,d7
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddSorted
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprAtDrawnOff - draw-list op: Op2_DrawSprAtDrawn plus a mirrored byte offset
; In:  a2 = draw list: byte dx, byte dy, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: byte offsets negated by flip bits, added to $4E/$50(a6); 12 runs
; ---------------------------------------------------------------------------
Op2_DrawSprAtDrawnOff:
	move.w	$38(a5),d6
	move.w	$3A(a5),d7
	move.b	(a2)+,d4
	move.b	(a2)+,d5
	ext.w	d4
	ext.w	d5
	btst	#$3,$38(a5)
	beq.s	loc_004902
	neg.w	d4
loc_004902:
	btst	#$4,$38(a5)
	beq.s	loc_00490C
	neg.w	d5
loc_00490C:
	add.w	$4E(a6),d4
	add.w	$50(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddSorted
	movea.w	(a2)+,a0
	jmp	(a0)
