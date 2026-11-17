; ---------------------------------------------------------------------------
; Span_ClearScreen - clears the whole span picture: all 224 lines get H-scroll 0 and V-scroll -line (Span_ClearLines)
; In:  a0 = H-scroll line table, a1 = V-scroll line table
; Out: as Span_ClearLines
; Evidence: `move.w #$E0,d7; moveq #0,d0; moveq #0,d2; jmp Span_ClearLines`; jmp from $334D2 (the stage 2 flying-section beam, 2 sites) when no shape is drawn; TAS frames 77185-79303
; ---------------------------------------------------------------------------
Span_ClearScreen:
	move.w	#$E0,d7
	moveq	#$0,d0
	moveq	#$0,d2
	jmp	Span_ClearLines(pc)
; ---------------------------------------------------------------------------
; Span_DrawTriangle - draws a filled triangle into the span tables: sorts the vertices g_TriX0/Y0 .. X2/Y2 by y, clears the lines above, fills both halves with Span_FillRamps (H = left edge, V = width - line, so each line shows a plane row as wide as the span; inferred) and clears the lines below, clipping at the top and at 224 lines
; In:  a0 = H-scroll line table (g_HScrollLinesA), a1 = V-scroll line table (g_HIntTblBack), g_TriX0..g_TriY2 = screen vertices
; Out: 224 lines written; g_SpanLinesLeft counted down by the lines drawn
; Evidence: three compare/exg sorts by y (then x), edge slopes by divs into 16.16, the width ramp minus $10000 per line, negative top y advanced by muls; called by $471C0 (mode $38 + HInt_InstallVScrollA) after computing the vertices with $154A6/$1543E/$153CE (inferred: a rotation/projection); 4 corpus runs incl. level-skip
; ---------------------------------------------------------------------------
Span_DrawTriangle:
	move.w	#$E0,(g_SpanLinesLeft).w
	move.w	(g_TriY0).w,d2
	move.w	(g_TriY1).w,d7
	move.w	(g_TriY2).w,d6
	move.w	(g_TriX0).w,d0
	move.w	(g_TriX1).w,d1
	move.w	(g_TriX2).w,d4
	cmp.w	d7,d2
	blt.s	loc_00F9A2
	bne.s	loc_00F99E
	cmp.w	d1,d0
	bgt.s	loc_00F9A2
loc_00F99E:
	exg.l	d2,d7
	exg.l	d0,d1
loc_00F9A2:
	cmp.w	d6,d7
	blt.s	loc_00F9B0
	bne.s	loc_00F9AC
	cmp.w	d4,d1
	bgt.s	loc_00F9B0
loc_00F9AC:
	exg.l	d6,d7
	exg.l	d1,d4
loc_00F9B0:
	cmp.w	d7,d2
	blt.s	loc_00F9BE
	bne.s	loc_00F9BA
	cmp.w	d1,d0
	bgt.s	loc_00F9BE
loc_00F9BA:
	exg.l	d2,d7
	exg.l	d0,d1
loc_00F9BE:
	sub.w	d2,d6
	ble.w	loc_00FB90
	move.w	d4,d3
	move.w	d4,d5
	sub.w	d1,d5
	sub.w	d0,d1
	sub.w	d0,d3
	swap	d0
	clr.w	d0
	sub.w	d2,d7
	bgt.s	loc_00F9DC
	swap	d1
	clr.w	d1
	bra.s	loc_00F9F8
loc_00F9DC:
	ext.l	d1
	cmpi.w	#$8,d7
	bgt.s	loc_00F9F0
	asl.l	#$5,d1
	divs.w	d7,d1
	ext.l	d1
	asl.l	#$3,d1
	asl.l	#$8,d1
	bra.s	loc_00F9F8
loc_00F9F0:
	asl.l	#$8,d1
	divs.w	d7,d1
	ext.l	d1
	asl.l	#$8,d1
loc_00F9F8:
	ext.l	d3
	asl.l	#$8,d3
	divs.w	d6,d3
	ext.l	d3
	asl.l	#$8,d3
	sub.w	d7,d6
	bgt.s	loc_00FA0C
	swap	d5
	clr.w	d5
	bra.s	loc_00FA16
loc_00FA0C:
	ext.l	d5
	asl.l	#$8,d5
	divs.w	d6,d5
	ext.l	d5
	asl.l	#$8,d5
loc_00FA16:
	cmp.l	d1,d3
	ble.s	loc_00FA26
	move.l	d5,d4
	move.l	d3,d5
	tst.w	d7
	bgt.s	loc_00FA2A
	add.l	d1,d0
	bra.s	loc_00FA2A
loc_00FA26:
	exg.l	d1,d3
	move.l	d1,d4
loc_00FA2A:
	sub.l	d1,d3
	sub.l	d4,d5
	ext.l	d2
	bpl.w	loc_00FB28
	add.w	d2,d7
	bmi.s	loc_00FA8E
	movem.l	d1/d3,-(a7)
	neg.w	d2
	movea.l	d1,a2
	tst.w	d1
	bpl.s	loc_00FA4A
	adda.l	#$10000,a2
loc_00FA4A:
	muls.w	d2,d1
	movea.l	d1,a3
	move.l	a2,d1
	swap	d1
	muls.w	d2,d1
	swap	d1
	clr.w	d1
	add.l	a3,d1
	movea.l	d3,a2
	tst.w	d3
	bpl.s	loc_00FA66
	adda.l	#$10000,a2
loc_00FA66:
	muls.w	d2,d3
	movea.l	d3,a3
	move.l	a2,d3
	swap	d3
	muls.w	d2,d3
	swap	d3
	clr.w	d3
	add.l	a3,d3
	move.l	d3,d2
	add.l	d1,d0
	movem.l	(a7)+,d1/d3
	addi.l	#$10000,d2
	tst.w	d7
	bgt.w	loc_00FB54
	bra.w	loc_00FB6C
loc_00FA8E:
	add.w	d7,d6
	ble.w	loc_00FB90
	movem.l	d4-d5,-(a7)
	sub.w	d7,d2
	neg.w	d7
	neg.w	d2
	movea.l	d1,a2
	tst.w	d1
	bpl.s	loc_00FAAA
	adda.l	#$10000,a2
loc_00FAAA:
	muls.w	d2,d1
	movea.l	d1,a3
	move.l	a2,d1
	swap	d1
	muls.w	d2,d1
	swap	d1
	clr.w	d1
	add.l	a3,d1
	movea.l	d4,a2
	tst.w	d4
	bpl.s	loc_00FAC6
	adda.l	#$10000,a2
loc_00FAC6:
	muls.w	d7,d4
	movea.l	d4,a3
	move.l	a2,d4
	swap	d4
	muls.w	d7,d4
	swap	d4
	clr.w	d4
	add.l	a3,d4
	movea.l	d5,a2
	tst.w	d5
	bpl.s	loc_00FAE2
	adda.l	#$10000,a2
loc_00FAE2:
	muls.w	d7,d5
	movea.l	d5,a3
	move.l	a2,d5
	swap	d5
	muls.w	d7,d5
	swap	d5
	clr.w	d5
	add.l	a3,d5
	tst.w	d2
	beq.s	loc_00FB12
	movea.l	d3,a2
	tst.w	d3
	bpl.s	loc_00FB02
	adda.l	#$10000,a2
loc_00FB02:
	muls.w	d2,d3
	movea.l	d3,a3
	move.l	a2,d3
	swap	d3
	muls.w	d2,d3
	swap	d3
	clr.w	d3
	add.l	a3,d3
loc_00FB12:
	add.l	d5,d3
	move.l	d3,d2
	add.l	d4,d1
	add.l	d1,d0
	movem.l	(a7)+,d4-d5
	addi.l	#$10000,d2
	bra.w	loc_00FB6C
loc_00FB28:
	cmpi.w	#$E0,d2
	bge.w	loc_00FB90
	sub.w	d2,(g_SpanLinesLeft).w
	movem.l	d0/d2/d7,-(a7)
	move.w	d2,d7
	moveq	#$0,d0
	moveq	#$0,d2
	jsr	Span_ClearLines(pc)
	movem.l	(a7)+,d0/d2/d7
	neg.l	d2
	addq.l	#$1,d2
	swap	d2
	tst.w	d7
	bgt.s	loc_00FB54
	add.l	d3,d2
	bra.s	loc_00FB6C
loc_00FB54:
	subi.l	#$10000,d3
	cmp.w	(g_SpanLinesLeft).w,d7
	ble.s	loc_00FB64
	move.w	(g_SpanLinesLeft).w,d7
loc_00FB64:
	sub.w	d7,(g_SpanLinesLeft).w
	jsr	Span_FillRamps(pc)
loc_00FB6C:
	subi.l	#$10000,d5
	tst.w	d6
	ble.s	loc_00FB90
	cmp.w	(g_SpanLinesLeft).w,d6
	ble.s	loc_00FB82
	move.w	(g_SpanLinesLeft).w,d6
	bmi.s	loc_00FBA2
loc_00FB82:
	sub.w	d6,(g_SpanLinesLeft).w
	move.w	d6,d7
	move.l	d4,d1
	move.l	d5,d3
	jsr	Span_FillRamps(pc)
loc_00FB90:
	move.w	(g_SpanLinesLeft).w,d7
	ble.s	loc_00FBA2
	move.w	#$FF20,d2
	add.w	d7,d2
	moveq	#$0,d0
	jmp	Span_ClearLines(pc)
loc_00FBA2:
	rts
; ---------------------------------------------------------------------------
; Span_ClearScreenAlt - second copy of Span_ClearScreen (224 lines of no span), placed before Span_DrawTriangleDither
; In:  a0 = H-scroll line table, a1 = V-scroll line table
; Out: as Span_ClearLines
; Evidence: byte-for-byte the same 3 instructions as Span_ClearScreen; jmp from $471C0 when its beam slot is empty
; ---------------------------------------------------------------------------
Span_ClearScreenAlt:
	move.w	#$E0,d7
	moveq	#$0,d0
	moveq	#$0,d2
	jmp	Span_ClearLines(pc)
; ---------------------------------------------------------------------------
; Span_DrawTriangleDither - Span_DrawTriangle with a half-width ramp and the dithered fills (Span_FillRampsDither0/1 by line and frame parity): draws the translucent beam triangle of the stage 2 flying-section boss
; In:  as Span_DrawTriangle
; Out: as Span_DrawTriangle
; Evidence: same vertex sort and clipping, then `asr.l #1` of both width slopes and a phase from g_SpanLinesLeft + g_FrameParity; jsr from $334D2 before Span_BuildCircle / Span_DrawDiscDither draw the disc at the beam's end; TAS frames 77185-79303 (stage 2 flying section, png f078000)
; ---------------------------------------------------------------------------
Span_DrawTriangleDither:
	move.w	#$E0,(g_SpanLinesLeft).w
	move.w	(g_TriY0).w,d2
	move.w	(g_TriY1).w,d7
	move.w	(g_TriY2).w,d6
	move.w	(g_TriX0).w,d0
	move.w	(g_TriX1).w,d1
	move.w	(g_TriX2).w,d4
	cmp.w	d7,d2
	blt.s	loc_00FBDC
	bne.s	loc_00FBD8
	cmp.w	d1,d0
	bgt.s	loc_00FBDC
loc_00FBD8:
	exg.l	d2,d7
	exg.l	d0,d1
loc_00FBDC:
	cmp.w	d6,d7
	blt.s	loc_00FBEA
	bne.s	loc_00FBE6
	cmp.w	d4,d1
	bgt.s	loc_00FBEA
loc_00FBE6:
	exg.l	d6,d7
	exg.l	d1,d4
loc_00FBEA:
	cmp.w	d7,d2
	blt.s	loc_00FBF8
	bne.s	loc_00FBF4
	cmp.w	d1,d0
	bgt.s	loc_00FBF8
loc_00FBF4:
	exg.l	d2,d7
	exg.l	d0,d1
loc_00FBF8:
	sub.w	d2,d6
	ble.w	loc_00FDFA
	move.w	d4,d3
	move.w	d4,d5
	sub.w	d1,d5
	sub.w	d0,d1
	sub.w	d0,d3
	swap	d0
	clr.w	d0
	sub.w	d2,d7
	bgt.s	loc_00FC16
	swap	d1
	clr.w	d1
	bra.s	loc_00FC32
loc_00FC16:
	ext.l	d1
	cmpi.w	#$8,d7
	bgt.s	loc_00FC2A
	asl.l	#$5,d1
	divs.w	d7,d1
	ext.l	d1
	asl.l	#$3,d1
	asl.l	#$8,d1
	bra.s	loc_00FC32
loc_00FC2A:
	asl.l	#$8,d1
	divs.w	d7,d1
	ext.l	d1
	asl.l	#$8,d1
loc_00FC32:
	ext.l	d3
	asl.l	#$8,d3
	divs.w	d6,d3
	ext.l	d3
	asl.l	#$8,d3
	sub.w	d7,d6
	bgt.s	loc_00FC46
	swap	d5
	clr.w	d5
	bra.s	loc_00FC50
loc_00FC46:
	ext.l	d5
	asl.l	#$8,d5
	divs.w	d6,d5
	ext.l	d5
	asl.l	#$8,d5
loc_00FC50:
	cmp.l	d1,d3
	ble.s	loc_00FC60
	move.l	d5,d4
	move.l	d3,d5
	tst.w	d7
	bgt.s	loc_00FC64
	add.l	d1,d0
	bra.s	loc_00FC64
loc_00FC60:
	exg.l	d1,d3
	move.l	d1,d4
loc_00FC64:
	sub.l	d1,d3
	sub.l	d4,d5
	asr.l	#$1,d3
	asr.l	#$1,d5
	ext.l	d2
	bpl.w	loc_00FD66
	add.w	d2,d7
	bmi.s	loc_00FCCC
	movem.l	d1/d3,-(a7)
	neg.w	d2
	movea.l	d1,a2
	tst.w	d1
	bpl.s	loc_00FC88
	adda.l	#$10000,a2
loc_00FC88:
	muls.w	d2,d1
	movea.l	d1,a3
	move.l	a2,d1
	swap	d1
	muls.w	d2,d1
	swap	d1
	clr.w	d1
	add.l	a3,d1
	movea.l	d3,a2
	tst.w	d3
	bpl.s	loc_00FCA4
	adda.l	#$10000,a2
loc_00FCA4:
	muls.w	d2,d3
	movea.l	d3,a3
	move.l	a2,d3
	swap	d3
	muls.w	d2,d3
	swap	d3
	clr.w	d3
	add.l	a3,d3
	move.l	d3,d2
	add.l	d1,d0
	movem.l	(a7)+,d1/d3
	addi.l	#$10000,d2
	tst.w	d7
	bgt.w	loc_00FD92
	bra.w	loc_00FDC4
loc_00FCCC:
	add.w	d7,d6
	ble.w	loc_00FDFA
	movem.l	d4-d5,-(a7)
	sub.w	d7,d2
	neg.w	d7
	neg.w	d2
	movea.l	d1,a2
	tst.w	d1
	bpl.s	loc_00FCE8
	adda.l	#$10000,a2
loc_00FCE8:
	muls.w	d2,d1
	movea.l	d1,a3
	move.l	a2,d1
	swap	d1
	muls.w	d2,d1
	swap	d1
	clr.w	d1
	add.l	a3,d1
	movea.l	d4,a2
	tst.w	d4
	bpl.s	loc_00FD04
	adda.l	#$10000,a2
loc_00FD04:
	muls.w	d7,d4
	movea.l	d4,a3
	move.l	a2,d4
	swap	d4
	muls.w	d7,d4
	swap	d4
	clr.w	d4
	add.l	a3,d4
	movea.l	d5,a2
	tst.w	d5
	bpl.s	loc_00FD20
	adda.l	#$10000,a2
loc_00FD20:
	muls.w	d7,d5
	movea.l	d5,a3
	move.l	a2,d5
	swap	d5
	muls.w	d7,d5
	swap	d5
	clr.w	d5
	add.l	a3,d5
	tst.w	d2
	beq.s	loc_00FD50
	movea.l	d3,a2
	tst.w	d3
	bpl.s	loc_00FD40
	adda.l	#$10000,a2
loc_00FD40:
	muls.w	d2,d3
	movea.l	d3,a3
	move.l	a2,d3
	swap	d3
	muls.w	d2,d3
	swap	d3
	clr.w	d3
	add.l	a3,d3
loc_00FD50:
	add.l	d5,d3
	move.l	d3,d2
	add.l	d4,d1
	add.l	d1,d0
	movem.l	(a7)+,d4-d5
	addi.l	#$10000,d2
	bra.w	loc_00FDC4
loc_00FD66:
	cmpi.w	#$E0,d2
	bge.w	loc_00FDFA
	sub.w	d2,(g_SpanLinesLeft).w
	movem.l	d0/d2/d7,-(a7)
	move.w	d2,d7
	moveq	#$0,d0
	moveq	#$0,d2
	jsr	Span_ClearLines(pc)
	movem.l	(a7)+,d0/d2/d7
	neg.l	d2
	addq.l	#$1,d2
	swap	d2
	tst.w	d7
	bgt.s	loc_00FD92
	add.l	d3,d2
	bra.s	loc_00FDC4
loc_00FD92:
	movem.l	d4-d6,-(a7)
	move.w	(g_SpanLinesLeft).w,d4
	add.b	(g_FrameParity).w,d4
	subi.l	#$10000,d3
	cmp.w	(g_SpanLinesLeft).w,d7
	ble.s	loc_00FDAE
	move.w	(g_SpanLinesLeft).w,d7
loc_00FDAE:
	sub.w	d7,(g_SpanLinesLeft).w
	lsr.w	#$1,d4
	bcc.s	loc_00FDBC
	jsr	Span_FillRampsDither1(pc)
	bra.s	loc_00FDC0
loc_00FDBC:
	jsr	Span_FillRampsDither0(pc)
loc_00FDC0:
	movem.l	(a7)+,d4-d6
loc_00FDC4:
	move.w	d6,d7
	move.l	d4,d1
	move.l	d5,d3
	move.w	(g_SpanLinesLeft).w,d4
	add.b	(g_FrameParity).w,d4
	subi.l	#$10000,d3
	tst.w	d7
	ble.s	loc_00FDFA
	cmp.w	(g_SpanLinesLeft).w,d7
	ble.s	loc_00FDE8
	move.w	(g_SpanLinesLeft).w,d7
	bmi.s	loc_00FE0C
loc_00FDE8:
	sub.w	d7,(g_SpanLinesLeft).w
	lsr.w	#$1,d4
	bcc.s	loc_00FDF6
	jsr	Span_FillRampsDither1(pc)
	bra.s	loc_00FDFA
loc_00FDF6:
	jsr	Span_FillRampsDither0(pc)
loc_00FDFA:
	move.w	(g_SpanLinesLeft).w,d7
	ble.s	loc_00FE0C
	move.w	#$FF20,d2
	add.w	d7,d2
	moveq	#$0,d0
	jmp	Span_ClearLines(pc)
loc_00FE0C:
	rts
