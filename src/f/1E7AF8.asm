; ---------------------------------------------------------------------------
; Snd_SendSfxBody - copies each pending sound-effect slot (nonzero long +2) to the Z80's request area $17EA+6n and clears the long; idle slots count their hold byte +1 down and then drop their priority +0
; In:  a0 = g_SndSfxSlots (6 slots x 6 bytes)
; Out: Z80 bus released; d0/d6/d7/a0/a1 trashed
; Evidence: bus request $A11100, a1 = $A017EA, 6 slots (dbra d7 #5), 6 bytes each with read-back verify; `dc.w $4228,$0000` = clr.b 0(a0); entered by Snd_SendSfx from the V-int every full frame; 146 corpus runs
; ---------------------------------------------------------------------------
Snd_SendSfxBody:
	move.w	#$100,($A11100).l
loc_1E7B00:
	btst	#$0,($A11100).l
	bne.s	loc_1E7B00
	lea.l	($A017EA).l,a1
	moveq	#$5,d7
loc_1E7B12:
	tst.l	$2(a0)
	beq.s	loc_1E7B3A
	moveq	#$5,d6
loc_1E7B1A:
	move.b	(a0)+,d0
loc_1E7B1C:
	move.b	d0,(a1)
	cmp.b	(a1),d0
	bne.s	loc_1E7B1C
	addq.w	#$1,a1
	dbra	d6,loc_1E7B1A
	clr.l	-$4(a0)
	dbra	d7,loc_1E7B12
	move.w	#$0,($A11100).l
	rts
loc_1E7B3A:
	tst.b	$1(a0)
	beq.s	loc_1E7B56
	subq.b	#$1,$1(a0)
	addq.w	#$6,a0
	addq.w	#$6,a1
	dbra	d7,loc_1E7B12
	move.w	#$0,($A11100).l
	rts
loc_1E7B56:
	dc.w	$4228,$0000	; insn: clr.b $0(a0)
	addq.w	#$6,a0
	addq.w	#$6,a1
	dbra	d7,loc_1E7B12
	move.w	#$0,($A11100).l
	rts
