; ---------------------------------------------------------------------------
; Stage4_InitConeRaster - scene setup for the translucent cones: scroll mode $38 (line H-scroll + the per-line V-scroll raster) and installs the H-int V-scroll handler for plane A
; In:  none
; Out: jumps to HInt_InstallVScrollA
; Evidence: called by Stage4_RunHall ($4000) and Stage4_RunFreezeFight ($4401), the scenes whose loops call Stage4_DrawCones; TAS f116191, f128791
; ---------------------------------------------------------------------------
Stage4_InitConeRaster:
	move.w	#$38,(g_ScrollMode).w
	jmp	(HInt_InstallVScrollA).l
; ---------------------------------------------------------------------------
; Stage4_DrawCones - per frame: picks the next of the three cone slots (g_ConeSlots, round robin) and draws it as a span triangle: apex = its screen point, the far edge = two points of width/length rotated by its two 3D angle sets (g_Rot3DOuter) and projected; also copies its colour set (Tbl_047508) into palette line 3 colours 10/11/12/14 unless fading; an idle slot clears the spans (twice, both buffers)
; In:  g_ConeSlots
; Out: g_TriX0..g_TriY2 and the span buffers (Span_DrawTriangle / Span_ClearScreenAlt)
; Evidence: Rot3D_BuildMatrix/TransformPoint/ProjectPoint on the slot's angles +8/+$A/+$C and +$E/+$10/+$12/+$14; g_FadeLevelVInt > -$A test before the palette write; called every frame by the $4000 and $4401 loops; slot owners are gas orbs (calls from $0421EC-$04226C) and Scr2_0451B6
; ---------------------------------------------------------------------------
Stage4_DrawCones:
	bra.w	loc_0471D0
loc_0471D0:
	move.w	(g_ConeDrawSlot).w,d0
	addq.w	#$1,d0
	cmpi.w	#$3,d0
	bcs.s	loc_0471DE
	moveq	#$0,d0
loc_0471DE:
	move.w	d0,(g_ConeDrawSlot).w
	lea.l	(g_ConeSlots).l,a6
	mulu.w	#$1E,d0
	adda.w	d0,a6
	tst.b	(a6)
	beq.w	loc_047318
	tst.w	$1C(a6)
	beq.w	loc_047318
	cmpi.b	#$F6,(g_FadeLevelVInt).w
	ble.s	loc_047232
	lea.l	(Tbl_047508).l,a0
	adda.w	$2(a6),a0
	move.w	(a0),(g_PalFaded+$74).w
	move.w	(a0)+,(g_PalSource+$74).w
	move.w	(a0),(g_PalFaded+$76).w
	move.w	(a0)+,(g_PalSource+$76).w
	move.w	(a0),(g_PalFaded+$78).w
	move.w	(a0)+,(g_PalSource+$78).w
	move.w	(a0),(g_PalFaded+$7C).w
	move.w	(a0)+,(g_PalSource+$7C).w
	st.b	(g_Pal3Changed).w
loc_047232:
	move.w	$4(a6),(g_TriX0).w
	move.w	$6(a6),(g_TriY0).w
	move.w	(g_ShakeY).w,d0
	sub.w	d0,(g_TriY0).w
	lea.l	(g_Rot3DOuter).l,a0
	clr.w	$C(a0)
	clr.w	$10(a0)
	clr.w	$14(a0)
	move.w	$8(a6),$4(a0)
	move.w	$A(a6),(a0)
	move.w	$C(a6),$8(a0)
	jsr	(Rot3D_BuildMatrix).l
	lea.l	(g_ConeEdge).l,a1
	move.w	$1C(a6),d0
	move.w	$1A(a6),d1
	asr.w	#$5,d0
	asr.w	#$4,d1
	move.w	d0,(a1)+
	move.w	d1,(a1)+
	clr.w	(a1)+
	neg.w	d0
	move.w	d0,(a1)+
	move.w	d1,(a1)+
	clr.w	(a1)+
	lea.l	-$C(a1),a1
	jsr	(Rot3D_TransformPoint).l
	move.w	d4,-$6(a1)
	move.w	d5,-$4(a1)
	move.w	d7,-$2(a1)
	jsr	(Rot3D_TransformPoint).l
	move.w	d4,-$6(a1)
	move.w	d5,-$4(a1)
	move.w	d7,-$2(a1)
	move.w	$E(a6),$4(a0)
	move.w	$10(a6),(a0)
	move.w	$12(a6),$8(a0)
	move.w	$14(a6),$14(a0)
	jsr	(Rot3D_BuildMatrix).l
	lea.l	(g_ConeEdge).l,a1
	jsr	(Rot3D_ProjectPoint).l
	add.w	(g_TriX0).w,d4
	add.w	(g_TriY0).w,d5
	move.w	d4,(g_TriX1).w
	move.w	d5,(g_TriY1).w
	jsr	(Rot3D_ProjectPoint).l
	add.w	(g_TriX0).w,d4
	add.w	(g_TriY0).w,d5
	move.w	d4,(g_TriX2).w
	move.w	d5,(g_TriY2).w
	clr.b	(g_ConeClearCount).w
	lea.l	(g_HScrollLinesA).l,a0
	movea.w	(g_HIntTblBack).w,a1
	jmp	(Span_DrawTriangle).l
loc_047318:
	cmpi.b	#$2,(g_ConeClearCount).w
	bne.s	loc_047322
	rts
loc_047322:
	addq.b	#$1,(g_ConeClearCount).w
	lea.l	(g_HScrollLinesA).l,a0
	movea.w	(g_HIntTblBack).w,a1
	jmp	(Span_ClearScreenAlt).l
; ---------------------------------------------------------------------------
; Stage4_ConeAlloc - a2 native: gives the object a free cone slot (pointer kept in $1E(a6)) starting at its position with width/length 0, depth $90 and the next of three colour sets; keeps an existing one
; In:  a6 = object (x/y $3E/$42)
; Out: d0 = 0 ok, 1 no free slot (for Op2_CallNativeRelSwitch)
; Evidence: callers: Op2_CallNativeRelSwitch at $04224E (gas orb) and $0451B6; scans the 3 slots at g_ConeSlots for a clear byte 0; cycles g_ConeNextColour 0-2 into +2 (x8)
; ---------------------------------------------------------------------------
Stage4_ConeAlloc:
	tst.w	$1E(a6)
	bne.s	loc_0473B6
	lea.l	(g_ConeSlots).l,a0
	moveq	#$2,d7
loc_047344:
	tst.b	(a0)
	beq.s	loc_047358
	lea.l	$1E(a0),a0
	dbra	d7,loc_047344
	clr.w	$1E(a6)
	moveq	#$1,d0
	rts
loc_047358:
	move.w	a0,$1E(a6)
	st.b	(a0)
	move.w	$3E(a6),$4(a0)
	move.w	$42(a6),$6(a0)
	moveq	#$0,d0
	move.b	d0,$1(a0)
	move.w	d0,$E(a0)
	move.w	d0,$10(a0)
	move.w	d0,$12(a0)
	move.w	d0,$8(a0)
	move.w	d0,$A(a0)
	move.w	d0,$C(a0)
	move.w	d0,$16(a0)
	move.w	d0,$18(a0)
	move.w	d0,$1A(a0)
	move.w	d0,$1C(a0)
	move.w	#$90,$14(a0)
	move.w	(g_ConeNextColour).w,d0
	addq.w	#$1,d0
	cmpi.w	#$3,d0
	bcs.s	loc_0473AC
	moveq	#$0,d0
loc_0473AC:
	move.w	d0,(g_ConeNextColour).w
	lsl.w	#$3,d0
	move.w	d0,$2(a0)
loc_0473B6:
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Stage4_ConeUpdate - a2 native, every frame of the cone's owner: moves the apex to the object's screen position, eases width +$1A toward +$16 (by $20) and length +$1C toward +$18 (by $1F), frees the slot when a closing cone (mode 3) reaches length 0, and steps the angles by mode: 0 spin with a random colour each frame, 1 one sweep turn following the heading, 2 sweep by the turn rate
; In:  a6 = object, a3 = its motion block (heading +$C, turn rate +$10)
; Out: slot updated (or freed)
; Evidence: callers: Op2_CallNativeRel at $0421EC and $042222 (gas orb loops); mode byte +1 set by Stage4_ConeSetInline; TAS RAM $F444-$F49D in 116520-119280 (widths $A0/$500/$6E0, lengths $20-$8C0)
; ---------------------------------------------------------------------------
Stage4_ConeUpdate:
	move.w	$1E(a6),d0
	beq.w	loc_0474BE
	movea.w	d0,a0
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	move.w	d0,$4(a0)
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	move.w	d0,$6(a0)
	move.w	$1A(a0),d0
	move.w	$16(a0),d1
	cmp.w	d1,d0
	blt.s	loc_0473F2
	subi.w	#$20,d0
	cmp.w	d1,d0
	bge.s	loc_0473FC
	bra.s	loc_0473FA
loc_0473F2:
	addi.w	#$20,d0
	cmp.w	d1,d0
	ble.s	loc_0473FC
loc_0473FA:
	move.w	d1,d0
loc_0473FC:
	move.w	d0,$1A(a0)
	move.w	$1C(a0),d0
	move.w	$18(a0),d1
	cmp.w	d1,d0
	blt.s	loc_047416
	subi.w	#$1F,d0
	cmp.w	d1,d0
	bge.s	loc_047420
	bra.s	loc_04741E
loc_047416:
	addi.w	#$1F,d0
	cmp.w	d1,d0
	ble.s	loc_047420
loc_04741E:
	move.w	d1,d0
loc_047420:
	move.w	d0,$1C(a0)
	bne.s	loc_047430
	cmpi.b	#$3,$1(a0)
	beq.w	Stage4_ConeFree
loc_047430:
	cmpi.b	#$0,$1(a0)
	beq.s	loc_047494
	cmpi.b	#$2,$1(a0)
	bne.s	loc_047462
	move.w	$10(a3),d0
	addi.w	#$B,d0
	add.w	d0,$A(a0)
	move.w	$C(a3),d0
	subi.w	#$100,d0
	move.w	d0,$E(a0)
	clr.w	$10(a0)
	clr.w	$12(a0)
	rts
loc_047462:
	move.w	$C(a3),d0
	subi.w	#$100,d0
	move.w	d0,$E(a0)
	clr.w	$10(a0)
	clr.w	$12(a0)
	move.w	$A(a0),d0
	andi.w	#$3FF,d0
	beq.s	loc_04748E
	addi.w	#$B,d0
	move.w	d0,d1
	andi.w	#$FC00,d1
	beq.s	loc_04748E
	moveq	#$0,d0
loc_04748E:
	move.w	d0,$A(a0)
	rts
loc_047494:
	addq.w	#$5,$E(a0)
	clr.w	$10(a0)
	addi.w	#$FFF1,$12(a0)
	clr.w	$8(a0)
	clr.w	$A(a0)
	clr.w	$C(a0)
	move.w	#$3,d0
	jsr	(Rand_Below).l
	lsl.w	#$3,d0
	move.w	d0,$2(a0)
loc_0474BE:
	rts
; ---------------------------------------------------------------------------
; Stage4_ConeSetInline - a2 native with inline arguments: byte colour (0 = keep, else colour set (n-8)/8), byte mode (0 spin, 1 sweep once, 2 sweep, 3 close), word target width, word target length; mode 2 also randomizes the sweep angle
; In:  a6 = object, a2 = script at the 6 argument bytes
; Out: a2 past the arguments (skipped when the object has no cone)
; Evidence: callers: Op2_CallNativeRel at $042214/$042258 and 24 times in $0451CA-$0452C6 (e.g. `0802 03C0 0640`, `1802 03C0 0780`); Tbl_047508 has 3 colour sets of 4 words
; ---------------------------------------------------------------------------
Stage4_ConeSetInline:
	move.w	$1E(a6),d0
	beq.s	loc_0474F4
	movea.w	d0,a0
	moveq	#$0,d0
	move.b	(a2)+,d0
	beq.s	loc_0474D4
	subq.w	#$8,d0
	move.w	d0,$2(a0)
loc_0474D4:
	move.b	(a2)+,d0
	move.b	d0,$1(a0)
	move.w	(a2)+,$16(a0)
	move.w	(a2)+,$18(a0)
	cmpi.b	#$2,d0
	bne.s	loc_0474F2
	jsr	(Rand_Next).l
	move.w	d0,$A(a0)
loc_0474F2:
	rts
loc_0474F4:
	addq.l	#$6,a2
	rts
; ---------------------------------------------------------------------------
; Stage4_ConeFree - a2 native: releases the object's cone slot ($1E(a6)) at once
; In:  a6 = object
; Out: slot byte 0 cleared, $1E(a6) = 0
; Evidence: callers: Op2_CallNativeRel at $042202/$042230/$04226C before Op2_Kill (gas orb deaths); Stage4_ConeUpdate branches here for a closed mode-3 cone
; ---------------------------------------------------------------------------
Stage4_ConeFree:
	move.w	$1E(a6),d0
	beq.s	loc_047506
	movea.w	d0,a0
	clr.w	$1E(a6)
	clr.b	(a0)
loc_047506:
	rts
