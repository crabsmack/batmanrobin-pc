; ---------------------------------------------------------------------------
; Span_DrawDiscDither - adds a dithered filled disc to the span tables, merging each line with a span already there (union of the two), from the half-width table g_CircleSpans
; In:  d0.w = radius (sign ignored), d1.w = centre x, d2.w = centre y (screen), a0 = H-scroll line table, a1 = V-scroll line table; g_CircleSpans built for this radius
; Out: the disc's lines updated (clipped to 0-223)
; Evidence: per line: H = (x - half-width) with bit 0 from the line + g_FrameParity parity; an empty line ((a1) + line = 0) takes the disc span, else the left edge and the V-scroll (width) are widened to cover both; upper half reads g_CircleSpans forward, lower half backward; jmp from $334D2 and Span_ClearDrawDisc; TAS frames 77312-79108 (the dithered bubble in png f077400/f078000)
; ---------------------------------------------------------------------------
Span_DrawDiscDither:
	tst.w	d0
	bpl.s	loc_00FE14
	neg.w	d0
loc_00FE14:
	lea.l	(g_CircleSpans).l,a2
	lea.l	$2(a2,d0.w),a3
	adda.w	d0,a3
	moveq	#$0,d6
	move.w	d2,d3
	move.w	d0,d7
	sub.w	d7,d3
	bpl.s	loc_00FE40
	add.w	d3,d7
	bgt.s	loc_00FE3A
	add.w	d7,d0
	ble.s	loc_00FE72
	asl.w	#$1,d7
	adda.w	d7,a3
	bra.w	loc_00FF0A
loc_00FE3A:
	asl.w	#$1,d3
	suba.w	d3,a2
	bra.s	loc_00FE4E
loc_00FE40:
	cmpi.w	#$E0,d3
	bge.s	loc_00FE72
	add.w	d3,d6
	add.w	d3,d3
	adda.w	d3,a0
	adda.w	d3,a1
loc_00FE4E:
	move.w	#$DF,d3
	sub.w	d6,d3
	sub.w	d7,d3
	bpl.s	loc_00FE5C
	add.w	d3,d7
	bmi.s	loc_00FE72
loc_00FE5C:
	swap	d0
	move.w	#$1,d0
	move.w	#$FFFE,d5
	move.w	d6,d2
	add.b	(g_FrameParity).w,d2
	lsr.b	#$1,d2
	bcc.s	loc_00FE76
	bra.s	loc_00FEC0
loc_00FE72:
	rts
loc_00FE74:
	addq.w	#$1,d6
loc_00FE76:
	move.w	(a2)+,d2
	move.w	d1,d3
	sub.w	d2,d3
	and.w	d5,d3
	move.w	(a1),d4
	add.w	d6,d4
	bne.s	loc_00FE8E
	move.w	d3,(a0)+
loc_00FE86:
	add.w	d2,(a1)+
	dbra	d7,loc_00FEBE
	bra.s	loc_00FF06
loc_00FE8E:
	sub.w	(a0),d3
	bpl.s	loc_00FEAC
	add.w	d3,(a0)+
	sub.w	d2,d4
	asr.w	#$1,d3
	cmp.w	d3,d4
	bmi.s	loc_00FEA4
	sub.w	d3,(a1)+
	dbra	d7,loc_00FEBE
	bra.s	loc_00FF06
loc_00FEA4:
	sub.w	d4,(a1)+
	dbra	d7,loc_00FEBE
	bra.s	loc_00FF06
loc_00FEAC:
	addq.l	#$2,a0
	asr.w	#$1,d3
	add.w	d3,d2
	sub.w	d4,d2
	bpl.s	loc_00FE86
	addq.l	#$2,a1
	dbra	d7,loc_00FEBE
	bra.s	loc_00FF06
loc_00FEBE:
	addq.w	#$1,d6
loc_00FEC0:
	move.w	(a2)+,d2
	move.w	d1,d3
	sub.w	d2,d3
	or.w	d0,d3
	move.w	(a1),d4
	add.w	d6,d4
	bne.s	loc_00FED8
	move.w	d3,(a0)+
loc_00FED0:
	add.w	d2,(a1)+
	dbra	d7,loc_00FE74
	bra.s	loc_00FF06
loc_00FED8:
	sub.w	(a0),d3
	bpl.s	loc_00FEF6
	add.w	d3,(a0)+
	sub.w	d2,d4
	asr.w	#$1,d3
	cmp.w	d3,d4
	bmi.s	loc_00FEEE
	sub.w	d3,(a1)+
	dbra	d7,loc_00FE74
	bra.s	loc_00FF06
loc_00FEEE:
	sub.w	d4,(a1)+
	dbra	d7,loc_00FE74
	bra.s	loc_00FF06
loc_00FEF6:
	addq.l	#$2,a0
	asr.w	#$1,d3
	add.w	d3,d2
	sub.w	d4,d2
	bpl.s	loc_00FED0
	addq.l	#$2,a1
	dbra	d7,loc_00FE74
loc_00FF06:
	addq.w	#$1,d6
	swap	d0
loc_00FF0A:
	move.w	#$DF,d3
	sub.w	d6,d3
	sub.w	d0,d3
	bpl.s	loc_00FF1A
	add.w	d3,d0
	bmi.w	loc_00FE72
loc_00FF1A:
	move.w	#$1,d7
	move.w	#$FFFE,d5
	move.w	d6,d2
	add.b	(g_FrameParity).w,d2
	lsr.b	#$1,d2
	bcc.s	loc_00FF30
	bra.s	loc_00FF7A
loc_00FF2E:
	addq.w	#$1,d6
loc_00FF30:
	move.w	-(a3),d2
	move.w	d1,d3
	sub.w	d2,d3
	and.w	d5,d3
	move.w	(a1),d4
	add.w	d6,d4
	bne.s	loc_00FF48
	move.w	d3,(a0)+
loc_00FF40:
	add.w	d2,(a1)+
	dbra	d0,loc_00FF78
	rts
loc_00FF48:
	sub.w	(a0),d3
	bpl.s	loc_00FF66
	add.w	d3,(a0)+
	sub.w	d2,d4
	asr.w	#$1,d3
	cmp.w	d3,d4
	bmi.s	loc_00FF5E
	sub.w	d3,(a1)+
	dbra	d0,loc_00FF78
	rts
loc_00FF5E:
	sub.w	d4,(a1)+
	dbra	d0,loc_00FF78
	rts
loc_00FF66:
	addq.l	#$2,a0
	asr.w	#$1,d3
	add.w	d3,d2
	sub.w	d4,d2
	bpl.s	loc_00FF40
	addq.l	#$2,a1
	dbra	d0,loc_00FF78
	rts
loc_00FF78:
	addq.w	#$1,d6
loc_00FF7A:
	move.w	-(a3),d2
	move.w	d1,d3
	sub.w	d2,d3
	or.w	d7,d3
	move.w	(a1),d4
	add.w	d6,d4
	bne.s	loc_00FF92
	move.w	d3,(a0)+
loc_00FF8A:
	add.w	d2,(a1)+
	dbra	d0,loc_00FF2E
	rts
loc_00FF92:
	sub.w	(a0),d3
	bpl.s	loc_00FFB0
	add.w	d3,(a0)+
	sub.w	d2,d4
	asr.w	#$1,d3
	cmp.w	d3,d4
	bmi.s	loc_00FFA8
	sub.w	d3,(a1)+
	dbra	d0,loc_00FF2E
	rts
loc_00FFA8:
	sub.w	d4,(a1)+
	dbra	d0,loc_00FF2E
	rts
loc_00FFB0:
	addq.l	#$2,a0
	asr.w	#$1,d3
	add.w	d3,d2
	sub.w	d4,d2
	bpl.s	loc_00FF8A
	addq.l	#$2,a1
	dbra	d0,loc_00FF2E
	rts
