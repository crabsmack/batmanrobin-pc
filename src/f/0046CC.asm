; ---------------------------------------------------------------------------
; Op2_DrawSprFront - draw-list op: draws one frame at the object's position in front of every sorted sprite (Spr_AddFront, no depth)
; In:  a2 = draw list: word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: Spr_ChangeFrame / bsr Spr_AddFront; 51 runs; $1E094 `46CC 32E6 44F2`
; ---------------------------------------------------------------------------
Op2_DrawSprFront:
	move.w	$38(a5),d6
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddFront
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprFrontAttr - draw-list op: Op2_DrawSprFront with the attribute byte change (and-mask, xor)
; In:  a2 = draw list: byte and-mask, byte xor, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: and.b/eor.b on $38(a5), Spr_AddFront; 30 runs; $28978 `46EA F810 3ACA`
; ---------------------------------------------------------------------------
Op2_DrawSprFrontAttr:
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
	bsr.w	Spr_AddFront
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprFrontOff - draw-list op: Op2_DrawSprFront at a mirrored byte offset from the object
; In:  a2 = draw list: byte dx, byte dy, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: byte offsets negated by flip bits 3/4, Spr_AddFront; 10 runs; $207DE `4710 000A 2CEC`
; ---------------------------------------------------------------------------
Op2_DrawSprFrontOff:
	move.w	$38(a5),d6
	move.b	(a2)+,d4
	move.b	(a2)+,d5
	ext.w	d4
	ext.w	d5
	btst	#$3,$38(a5)
	beq.s	loc_004726
	neg.w	d4
loc_004726:
	btst	#$4,$38(a5)
	beq.s	loc_004730
	neg.w	d5
loc_004730:
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddFront
	movea.w	(a2)+,a0
	jmp	(a0)
