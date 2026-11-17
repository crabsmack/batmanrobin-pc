; ---------------------------------------------------------------------------
; Lz_Decompress - LZ77 decompressor (the background task's code): copies literals from a separate stream and matches from the output until the end code, then ends the background task
; In:  a0 = packed block (long offset to the literal stream, then 16-bit MSB-first control words), a1 = destination
; Out: a1 = output end, also stored to g_LzOutEnd; g_BgTaskActive and g_BgTaskRunning cleared (under SR $2700); uses d1-d2/d6-d7/a2-a3
; Evidence: control bit 0 = literal (Lz_Decompress_Literal, byte from the a2 stream), 1 = match: length 2, or 2/4/8-bit tiers adding 1/4/$13 (2-275 bytes; an 8-bit 0 is the end code -> Lz_Decompress_Done), then a 2-bit distance class with 5/7/9/10 bits plus 1/$21/$A1/$2A1; entered by BgTask_Start's context (a4 = $A23E) and by jsr from the a6 op $7108; tools/lz.py models it and unpacks all 103 blocks; NOTES 'Decompressor'; TAS 75 frames
; ---------------------------------------------------------------------------
Lz_Decompress:
	movea.l	a0,a2
	adda.l	(a0)+,a2
	bra.s	Lz_Decompress_Refill
Lz_Decompress_Literal:
	move.b	(a2)+,(a1)+
Lz_Decompress_Next:
	dbra	d7,Lz_Decompress_TestBit
Lz_Decompress_Refill:
	move.w	(a0)+,d1
	swap	d1
	moveq	#$F,d7
Lz_Decompress_TestBit:
	add.l	d1,d1
	bcc.s	Lz_Decompress_Literal
	moveq	#$1,d2
	dbra	d7,loc_00A260
	move.w	(a0)+,d1
	swap	d1
	moveq	#$F,d7
loc_00A260:
	add.l	d1,d1
	bcc.s	Lz_Decompress_Distance
	moveq	#$2,d6
	clr.w	d1
	sub.w	d6,d7
	bgt.s	loc_00A27C
	add.w	d6,d7
	sub.w	d7,d6
	rol.l	d7,d1
	swap	d1
	move.w	(a0)+,d1
	swap	d1
	moveq	#$10,d7
	sub.w	d6,d7
loc_00A27C:
	rol.l	d6,d1
	tst.w	d1
	bne.s	loc_00A2C4
	moveq	#$4,d2
	moveq	#$4,d6
	clr.w	d1
	sub.w	d6,d7
	bgt.s	loc_00A29C
	add.w	d6,d7
	sub.w	d7,d6
	rol.l	d7,d1
	swap	d1
	move.w	(a0)+,d1
	swap	d1
	moveq	#$10,d7
	sub.w	d6,d7
loc_00A29C:
	rol.l	d6,d1
	tst.w	d1
	bne.s	loc_00A2C4
	moveq	#$13,d2
	moveq	#$8,d6
	clr.w	d1
	sub.w	d6,d7
	bgt.s	loc_00A2BC
	add.w	d6,d7
	sub.w	d7,d6
	rol.l	d7,d1
	swap	d1
	move.w	(a0)+,d1
	swap	d1
	moveq	#$10,d7
	sub.w	d6,d7
loc_00A2BC:
	rol.l	d6,d1
	tst.w	d1
	beq.w	Lz_Decompress_Done
loc_00A2C4:
	add.w	d1,d2
Lz_Decompress_Distance:
	moveq	#$2,d6
	clr.w	d1
	sub.w	d6,d7
	bgt.s	loc_00A2DE
	add.w	d6,d7
	sub.w	d7,d6
	rol.l	d7,d1
	swap	d1
	move.w	(a0)+,d1
	swap	d1
	moveq	#$10,d7
	sub.w	d6,d7
loc_00A2DE:
	rol.l	d6,d1
	subq.w	#$1,d1
	bcc.s	loc_00A30E
	moveq	#$5,d6
	clr.w	d1
	sub.w	d6,d7
	bgt.s	loc_00A2FC
	add.w	d6,d7
	sub.w	d7,d6
	rol.l	d7,d1
	swap	d1
	move.w	(a0)+,d1
	swap	d1
	moveq	#$10,d7
	sub.w	d6,d7
loc_00A2FC:
	rol.l	d6,d1
	addq.w	#$1,d1
	movea.l	a1,a3
	suba.w	d1,a3
loc_00A304:
	move.b	(a3)+,(a1)+
	dbra	d2,loc_00A304
	bra.w	Lz_Decompress_Next
loc_00A30E:
	bne.s	loc_00A33C
	moveq	#$7,d6
	clr.w	d1
	sub.w	d6,d7
	bgt.s	loc_00A328
	add.w	d6,d7
	sub.w	d7,d6
	rol.l	d7,d1
	swap	d1
	move.w	(a0)+,d1
	swap	d1
	moveq	#$10,d7
	sub.w	d6,d7
loc_00A328:
	rol.l	d6,d1
	addi.w	#$21,d1
	movea.l	a1,a3
	suba.w	d1,a3
loc_00A332:
	move.b	(a3)+,(a1)+
	dbra	d2,loc_00A332
	bra.w	Lz_Decompress_Next
loc_00A33C:
	subq.w	#$1,d1
	bne.s	loc_00A36C
	moveq	#$9,d6
	clr.w	d1
	sub.w	d6,d7
	bgt.s	loc_00A358
	add.w	d6,d7
	sub.w	d7,d6
	rol.l	d7,d1
	swap	d1
	move.w	(a0)+,d1
	swap	d1
	moveq	#$10,d7
	sub.w	d6,d7
loc_00A358:
	rol.l	d6,d1
	addi.w	#$A1,d1
	movea.l	a1,a3
	suba.w	d1,a3
loc_00A362:
	move.b	(a3)+,(a1)+
	dbra	d2,loc_00A362
	bra.w	Lz_Decompress_Next
loc_00A36C:
	moveq	#$A,d6
	clr.w	d1
	sub.w	d6,d7
	bgt.s	loc_00A384
	add.w	d6,d7
	sub.w	d7,d6
	rol.l	d7,d1
	swap	d1
	move.w	(a0)+,d1
	swap	d1
	moveq	#$10,d7
	sub.w	d6,d7
loc_00A384:
	rol.l	d6,d1
	addi.w	#$2A1,d1
	movea.l	a1,a3
	suba.w	d1,a3
loc_00A38E:
	move.b	(a3)+,(a1)+
	dbra	d2,loc_00A38E
	bra.w	Lz_Decompress_Next
Lz_Decompress_Done:
	move.l	a1,(g_LzOutEnd).w
	ori.w	#$700,sr
	clr.b	(g_BgTaskActive).w
	clr.b	(g_BgTaskRunning).w
	andi.w	#$F8FF,sr
	rts
