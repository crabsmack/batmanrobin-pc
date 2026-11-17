; ---------------------------------------------------------------------------
; Flight_DrawCorridor - draws the flying section's corridor into plane B's line H-scroll and the H-int line V-scroll table: alternating perspective ceiling/floor spans (Span_FillRamps), bands of the middle wall (Flight_CopyBandLines) and plain fill, until 224 lines are done
; In:  g_SceneVars +0/+4/+$12.. from Flight_UpdateCorridor
; Out: g_HScrollLinesB and (g_HIntTblBack) filled; g_SceneVars+8/+$C/+$10 work values
; Evidence: TAS f35433-f47954 every frame (11967 frames), level-skip f4424-f5883; line budget g_SceneVars+$10 = $E0
; ---------------------------------------------------------------------------
Flight_DrawCorridor:
	lea.l	(g_HScrollLinesB).l,a0
	movea.w	(g_HIntTblBack).w,a1
	move.w	#$E0,(g_SceneVars+$10).w
	move.l	(g_SceneVars+$4).w,d4
	move.l	d4,d5
	asr.l	#$2,d5
	move.l	d5,(g_SceneVars+$8).w
	sub.l	d5,d4
	asr.l	#$8,d4
	move.l	d4,(g_SceneVars+$C).w
	move.w	(g_SceneVars).l,d4
	move.w	d4,d5
	asr.w	#$2,d5
	addi.w	#$54,d5
	subi.w	#$40,d5
	subi.w	#$100,d4
loc_02493A:
	move.w	d5,d6
	sub.w	d4,d6
	ble.s	loc_0249B8
	move.w	d6,d7
	move.w	d4,d2
	addi.w	#$80,d2
	sub.w	d5,d2
	bpl.s	loc_02494E
	add.w	d2,d7
loc_02494E:
	move.w	d4,d2
	neg.w	d2
	bpl.s	loc_024956
	moveq	#$0,d2
loc_024956:
	sub.w	d2,d7
	ble.s	loc_0249B8
	move.w	d2,d0
	move.l	#$5000,d3
	divu.w	d6,d3
	mulu.w	d3,d2
	subi.w	#$100,d3
	ext.l	d3
	asl.l	#$8,d3
	asl.l	#$8,d2
	swap	d2
	addi.w	#$FFA0,d2
	add.w	(g_SceneVars+$10).w,d2
	move.l	(g_SceneVars+$C).l,d1
	divs.w	d6,d1
	neg.w	d1
	muls.w	d1,d0
	ext.l	d1
	asl.l	#$8,d1
	asl.l	#$8,d0
	add.l	(g_SceneVars+$4).l,d0
	swap	d2
	subi.l	#$8000,d2
	subi.l	#$8000,d0
	sub.w	d7,(g_SceneVars+$10).w
	bgt.s	loc_0249B0
	add.w	(g_SceneVars+$10).w,d7
	jmp	(Span_FillRamps).l
loc_0249B0:
	jsr	(Span_FillRamps).l
	swap	d2
loc_0249B8:
	move.w	d4,d3
	move.w	d5,d2
	addi.w	#$80,d4
	addi.w	#$20,d5
	move.w	d2,d7
	sub.w	d3,d2
	ble.s	loc_0249CC
	moveq	#$0,d2
loc_0249CC:
	sub.w	d2,d7
	bpl.s	loc_0249D2
	add.w	d7,d2
loc_0249D2:
	move.w	d4,d7
	sub.w	d5,d7
	ble.s	loc_0249DA
	moveq	#$0,d7
loc_0249DA:
	add.w	d2,d7
	addi.w	#$20,d7
	ble.s	loc_024A02
	lea.l	(g_SceneVars+$12).l,a4
	move.w	d2,d3
	lsl.w	#$2,d3
	suba.w	d3,a4
	neg.w	d2
	addi.w	#$FFF0,d2
	add.w	(g_SceneVars+$10).w,d2
	move.w	(g_SceneVars+$8).l,d0
	bsr.w	Flight_CopyBandLines
loc_024A02:
	move.w	d4,d6
	sub.w	d5,d6
	ble.s	loc_024A78
	move.w	d6,d7
	move.w	d4,d2
	subi.w	#$80,d2
	sub.w	d5,d2
	bpl.s	loc_024A16
	moveq	#$0,d2
loc_024A16:
	sub.w	d2,d7
	ble.s	loc_024A78
	move.w	d2,d0
	move.l	#$5000,d3
	divu.w	d6,d3
	neg.w	d3
	muls.w	d3,d2
	subi.w	#$100,d3
	ext.l	d3
	asl.l	#$8,d3
	asl.l	#$8,d2
	swap	d2
	addi.w	#$FFEF,d2
	add.w	(g_SceneVars+$10).w,d2
	move.l	(g_SceneVars+$C).l,d1
	divs.w	d6,d1
	muls.w	d1,d0
	ext.l	d1
	asl.l	#$8,d1
	asl.l	#$8,d0
	add.l	(g_SceneVars+$8).l,d0
	swap	d2
	addi.l	#$8000,d2
	addi.l	#$8000,d0
	sub.w	d7,(g_SceneVars+$10).w
	bgt.s	loc_024A70
	add.w	(g_SceneVars+$10).w,d7
	jmp	(Span_FillRamps).l
loc_024A70:
	jsr	(Span_FillRamps).l
	swap	d2
loc_024A78:
	move.w	d4,d3
	addi.w	#$80,d4
	addi.w	#$20,d5
	move.w	d4,d7
	sub.w	d3,d7
	ble.w	loc_02493A
	moveq	#$0,d2
	tst.w	d3
	bpl.s	loc_024A94
	add.w	d3,d7
	sub.w	d3,d2
loc_024A94:
	addi.w	#$FF20,d2
	add.w	(g_SceneVars+$10).w,d2
	move.w	(g_SceneVars+$4).l,d0
	sub.w	d7,(g_SceneVars+$10).w
	bgt.s	loc_024AB2
	add.w	(g_SceneVars+$10).w,d7
	jmp	(Mem_FillWords2).l
loc_024AB2:
	jsr	(Mem_FillWords2).l
	bra.w	loc_02493A
