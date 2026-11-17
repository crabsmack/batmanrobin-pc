; ---------------------------------------------------------------------------
; Op2_DrawInScreenBox - a2 op: draws the sprite (as Op2_Draw) only while its screen position (x - g_CameraX, y - g_CameraY) lies in x [left, right] and y [top, bottom)
; In:  a2 = script: words left, right, top, bottom (screen pixels)
; Out: NEXT after the operands, drawn or not
; Evidence: cmp/blt/bgt chain then fall into Op2_Draw ($448A), else skip the remaining words; 47 corpus runs; $13358 `4454 FFF6 014A FFF6 014E`
; ---------------------------------------------------------------------------
Op2_DrawInScreenBox:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	cmp.w	(a2)+,d0
	blt.s	loc_004484
	cmp.w	(a2)+,d0
	bgt.s	loc_00447E
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	cmp.w	(a2)+,d0
	blt.s	loc_004478
	cmp.w	(a2)+,d0
	blt.s	Op2_Draw
	movea.w	(a2)+,a0
	jmp	(a0)
loc_004478:
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_00447E:
	addq.l	#$4,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_004484:
	addq.l	#$6,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_Draw - a2 op: draws the current sprite block by running its draw list $2A(a5) (sprite ops from Op2_AnimFrame) with a1 = its frame slots; the blink timer $16(a5) hides it on odd frames and the shake timer $17(a5) jitters x/y by -2..1 px while they count down
; In:  a2 = script (no operand); a5 = sprite block
; Out: NEXT (Op2_DrawEnd returns to the next op's handler pushed here)
; Evidence: movea.w (a2)+,a0 / pea (a0) / pea (a2) / movea.l $2A(a5),a2 / lea (a5),a1; blink: g_FrameParity skip; shake: Rand_Next asl 2 / swap, restored by Op2_Draw_RestorePos; 153 corpus runs; $13312 `448A`
; ---------------------------------------------------------------------------
Op2_Draw:
	tst.w	$16(a5)
	bne.s	loc_0044A0
loc_004490:
	movea.w	(a2)+,a0
	pea.l	(a0)
	pea.l	(a2)
	movea.l	$2A(a5),a2
	lea.l	(a5),a1
	movea.w	(a2)+,a0
	jmp	(a0)
loc_0044A0:
	tst.b	$16(a5)
	beq.s	loc_0044B2
	subq.b	#$1,$16(a5)
	tst.b	(g_FrameParity).w
	bne.w	VM2_Next
loc_0044B2:
	tst.b	$17(a5)
	beq.s	loc_004490
	subq.b	#$1,$17(a5)
	bsr.w	Rand_Next
	asl.l	#$2,d0
	swap	d0
	move.w	$3E(a6),-(a7)
	move.w	$42(a6),-(a7)
	add.w	d0,$3E(a6)
	add.w	d0,$42(a6)
	pea.l	(Op2_Draw_RestorePos).l
	pea.l	(a2)
	movea.l	$2A(a5),a2
	lea.l	(a5),a1
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_Draw_RestorePos - continuation of a shaken Op2_Draw: restores the object's x/y words saved before the jitter and continues the script; entered by rts from Op2_DrawEnd, not dispatched from scripts
; In:  sp = saved y word, x word; a2 = script
; Out: $3E/$42(a6) restored; NEXT
; Evidence: pea.l $44E6 in the shake path; 77 corpus runs as a computed target
; ---------------------------------------------------------------------------
Op2_Draw_RestorePos:
	move.w	(a7)+,$42(a6)
	move.w	(a7)+,$3E(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawEnd - draw-list op: ends a draw list: restores the object script's a2, sets the sprite block's frame count $14(a5) to the slots used this time and releases the frames of the slots no longer used
; In:  a1 = next frame slot; (sp) = object script a2, then the return handler pushed by Op2_Draw
; Out: rts into that handler (NEXT)
; Evidence: movea.l (a7)+,a2 / (a1-a5)/2 -> $14(a5) / Spr_ReleaseFrame + clr.w (a1)+ for the rest; 153 runs; 1837 script uses (`... 4524 2A80 44F2`)
; ---------------------------------------------------------------------------
Op2_DrawEnd:
	movea.l	(a7)+,a2
	move.w	a1,d0
	sub.w	a5,d0
	asr.w	#$1,d0
	move.w	$14(a5),d7
	move.w	d0,$14(a5)
	sub.w	d0,d7
	bgt.s	loc_004508
	rts
loc_004508:
	move.w	(a1),d2
	bsr.w	Spr_ReleaseFrame
	clr.w	(a1)+
	subq.w	#$2,d7
	bpl.s	loc_004516
	rts
loc_004516:
	move.w	(a1),d2
	bsr.w	Spr_ReleaseFrame
	clr.w	(a1)+
	dbra	d7,loc_004516
	rts
; ---------------------------------------------------------------------------
; Op2_DrawSpr - draw-list op: draws one sprite frame at the object's position with the block's attribute $38(a5) and depth $3A(a5): Spr_ChangeFrame on the next frame slot (keeps art loaded), then Spr_AddSorted
; In:  a2 = draw list: word frame; a1 = frame slot
; Out: slot updated, a1 + 2; NEXT
; Evidence: move.w (a2)+,d1 / move.w (a1),d2 / bsr Spr_ChangeFrame / move.w d1,(a1)+ / bsr Spr_AddSorted; 152 runs; 1672 uses, e.g. $13462 `4524 2A80 44F2`
; ---------------------------------------------------------------------------
Op2_DrawSpr:
	move.w	$38(a5),d6
	move.w	$3A(a5),d7
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddSorted
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprAttr - draw-list op: Op2_DrawSpr with the attribute's high byte (priority, palette, flips) changed: (attr & mask) ^ xor
; In:  a2 = draw list: byte and-mask, byte xor, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: and.b (a2)+,d6 / eor.b d0,d6 / lsl.w #8,d6 then Spr_ChangeFrame / Spr_AddSorted; 103 runs; $19B0A `4546 18C0 FC90`
; ---------------------------------------------------------------------------
Op2_DrawSprAttr:
	move.w	$3A(a5),d7
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
	bsr.w	Spr_AddSorted
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprDepth - draw-list op: Op2_DrawSpr with a depth offset added to $3A(a5) (sorts this piece in front of or behind the rest)
; In:  a2 = draw list: word depth offset, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: move.w $3A(a5),d7 / add.w (a2)+,d7; 87 runs; $1B4FE `4570 1000 2FB2`
; ---------------------------------------------------------------------------
Op2_DrawSprDepth:
	move.w	$38(a5),d6
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
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
; Op2_DrawSprAttrDepth - draw-list op: Op2_DrawSpr with both the attribute byte change (and-mask, xor) and a depth offset
; In:  a2 = draw list: byte and-mask, byte xor, word depth offset, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: the Attr and Depth code combined; 2 runs; $19B18 `4594 18C0 FFCE FC9A`
; ---------------------------------------------------------------------------
Op2_DrawSprAttrDepth:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
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
; Op2_DrawSprOff - draw-list op: Op2_DrawSpr at the object's position plus a signed byte offset, mirrored by the H/V flip bits 3/4 of $38(a5)
; In:  a2 = draw list: byte dx, byte dy, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: move.b (a2)+,d4/d5 / ext / neg on btst #3/#4,$38(a5); 109 runs; 322 uses, e.g. $19D1A `45C0 0100 FC7C`
; ---------------------------------------------------------------------------
Op2_DrawSprOff:
	move.w	$38(a5),d6
	move.w	$3A(a5),d7
	move.b	(a2)+,d4
	move.b	(a2)+,d5
	ext.w	d4
	ext.w	d5
	btst	#$3,$38(a5)
	beq.s	loc_0045DA
	neg.w	d4
loc_0045DA:
	btst	#$4,$38(a5)
	beq.s	loc_0045E4
	neg.w	d5
loc_0045E4:
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddSorted
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DrawSprAttrOff - draw-list op: Op2_DrawSpr with the attribute byte change and a mirrored byte offset
; In:  a2 = draw list: byte and-mask, byte xor, byte dx, byte dy, word frame; a1 = frame slot
; Out: a1 + 2; NEXT
; Evidence: Attr then Off code, Spr_AddSorted; 75 runs; $1366A `45FE F008 0100 2B16`
; ---------------------------------------------------------------------------
Op2_DrawSprAttrOff:
	move.w	$3A(a5),d7
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
	beq.s	loc_004620
	neg.w	d4
loc_004620:
	btst	#$4,$38(a5)
	beq.s	loc_00462A
	neg.w	d5
loc_00462A:
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	move.w	(a2)+,d1
	move.w	(a1),d2
	bsr.w	Spr_ChangeFrame
	move.w	d1,(a1)+
	bsr.w	Spr_AddSorted
	movea.w	(a2)+,a0
	jmp	(a0)
