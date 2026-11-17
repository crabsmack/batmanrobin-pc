; ---------------------------------------------------------------------------
; ToyRing_ReleaseFrames - releases the sprite frames of all the figures of a toy ring (one word per figure from +$24 of the ring block)
; In:  a6 = ring object, a4 +$24 = figure count
; Out: frames released (Spr_ReleaseFrame), a1 past the list
; Evidence: dbra over $24(a4) from $E(a6)+$24; called by ToyRing_StartBreakUp and from the death path $03762A before Op2_Kill; TAS f79748-f112915 (25 frames)
; ---------------------------------------------------------------------------
ToyRing_ReleaseFrames:
	movea.w	$E(a6),a1
	move.w	$24(a4),d7
	lea.l	$24(a1),a1
	bra.s	loc_03771E
loc_037716:
	move.w	(a1)+,d2
	jsr	(Spr_ReleaseFrame).l
loc_03771E:
	dbra	d7,loc_037716
	rts
; ---------------------------------------------------------------------------
; ToyRing_SetSquashSpeedArg - sets a toy ring's squash speed (+$C of the ring block, word << 8) from g_VM2Arg0
; In:  g_VM2Arg0 = speed word, a6 = ring object
; Out: $C(ring block) = speed << 8 (falls into ToyRing_SetSquashSpeed's tail)
; Evidence: move.w g_VM2Arg0,d0 / asl.l #8 / move.l d0,$C(a0); Op2_CallNativeRel at $037670 (landing coroutine of Scr2_037588)
; ---------------------------------------------------------------------------
ToyRing_SetSquashSpeedArg:
	move.w	(g_VM2Arg0).w,d0
	bra.s	loc_03772C
; ---------------------------------------------------------------------------
; ToyRing_SetSquashSpeed - sets a toy ring's squash speed from the next script word (word << 8), e.g. $FA00 on landing so the ring flattens and springs back
; In:  a2 = script (word), a6 = ring object
; Out: a2 past the word; $C(ring block) = speed << 8
; Evidence: move.w (a2)+,d0 / asl.l #8; Op2_CallNativeRel $03764E with word $FA00 after the floor test in the hop coroutine of Scr2_037604
; ---------------------------------------------------------------------------
ToyRing_SetSquashSpeed:
	move.w	(a2)+,d0
loc_03772C:
	movea.w	$E(a6),a0
	ext.l	d0
	asl.l	#$8,d0
	move.l	d0,$C(a0)
	rts
; ---------------------------------------------------------------------------
; ToyRing_TestRebound - switch native: reports whether a landed toy ring has sprung back (squash +$8 >= 0 and squash speed +$C >= 0)
; In:  a6 = ring object
; Out: d0 = 1 rebounding, 0 still compressed
; Evidence: tst.w $8(a0) / tst.w $C(a0); Op2_CallNativeRelSwitch $037680 in the landing coroutine, case 1 jumps again (Op2_GotoVector0)
; ---------------------------------------------------------------------------
ToyRing_TestRebound:
	movea.w	$E(a6),a0
	tst.w	$8(a0)
	bmi.s	loc_03774E
	tst.w	$C(a0)
	bmi.s	loc_03774E
	moveq	#$1,d0
	rts
loc_03774E:
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; ToyRing_Init - inline-argument native that sets up a toy ring (a hoop of linked figures): words figure count, radius, spring stiffness, blink reload, blink pattern; the angle step between figures = $4000 / count
; In:  a2 = script (5 words), a6 = ring object
; Out: a2 past the words; a4 +$24 = count; ring block $E(a6) cleared then +0/+4 radius, +$10 stiffness << 8, +$18 angle step, +$20/+$22 blink timer, +$1C blink pattern
; Evidence: ObjPool_Clear; divu.w d0 of #$4000; first op of the ring spawner scripts, e.g. Scr2_03905E: 16 figures radius $6E (energy $400), Scr2_0391F4: 6 figures; then Op2_Goto Scr2_037588 (the shared ring body)
; ---------------------------------------------------------------------------
ToyRing_Init:
	movea.w	$E(a6),a0
	jsr	(ObjPool_Clear).l
	move.w	(a2)+,d0
	move.w	(a2)+,d1
	move.w	(a2)+,d2
	ext.l	d2
	asl.l	#$8,d2
	move.w	d0,$24(a4)
	move.w	d1,(a0)
	move.w	d1,$4(a0)
	move.l	d2,$10(a0)
	move.l	#$4000,d1
	divu.w	d0,d1
	swap	d1
	clr.w	d1
	lsr.l	#$4,d1
	move.l	d1,$18(a0)
	move.w	(a2)+,d0
	move.w	d0,$20(a0)
	move.w	d0,$22(a0)
	move.w	(a2)+,$1C(a0)
	rts
; ---------------------------------------------------------------------------
; ToyRing_Update - per-frame toy-ring update: runs the squash spring (+$8 squash, +$C its speed, stiffness +$10, damped on overshoot), rolls the ring with its x speed (spin +$14 += x speed / radius), sizes its body and attack boxes to the squashed radii (no attack unless charged), keeps its bottom on the floor while it squashes, and draws every figure round the ellipse facing outward
; In:  a6 = ring object (a3/a4/a5 its blocks), $E(a6) = ring block
; Out: a2-a6 preserved; box rects, a3 +$42/+$4A terrain offsets, y $42(a6), figure frames and sprites (Spr_AddFront)
; Evidence: radii r - squash/2 (x) and r + squash (y); divu (a2) of #$10000 times $18(a3); movem.w d2-d5,$8(box) for $4(a4) and $C(a4); move.b #2,$1(a4) unless var 0 bit 2; Mot_Atan2 per figure + $200; Op2_CallNativeRel $037618 in the ring loop Scr2_037604; screenshot s82020: five linked figures in a hoop
; ---------------------------------------------------------------------------
ToyRing_Update:
	movem.l	a2-a6,-(a7)
	movea.w	$E(a6),a2
	move.l	$C(a2),d0
	move.l	$8(a2),d1
	bpl.s	loc_0377C0
	add.l	$10(a2),d0
	add.l	d0,d1
	bpl.s	loc_0377C8
	move.l	(a2),d2
	move.l	d2,d3
	asr.l	#$3,d3
	sub.l	d3,d2
	add.l	d1,d2
	bpl.s	loc_0377E6
	moveq	#$0,d0
	bra.s	loc_0377E6
loc_0377C0:
	sub.l	$10(a2),d0
	add.l	d0,d1
	bpl.s	loc_0377E6
loc_0377C8:
	move.l	d0,d2
	asr.l	#$2,d2
	sub.l	d2,d0
	bpl.s	loc_0377DA
	cmpi.l	#$FFFF0000,d0
	blt.s	loc_0377E6
	bra.s	loc_0377E2
loc_0377DA:
	cmpi.l	#$10000,d0
	bgt.s	loc_0377E6
loc_0377E2:
	moveq	#$0,d0
	moveq	#$0,d1
loc_0377E6:
	move.l	d0,$C(a2)
	move.l	d1,$8(a2)
	move.w	$22(a4),d0
	andi.w	#$C,d0
	beq.s	loc_037830
	andi.w	#$4,d0
	bne.s	loc_03781A
	move.w	(g_FrameCounter+2).w,d0
	andi.w	#$8,d0
	beq.s	loc_037830
	jsr	(Rand_Next).l
	andi.w	#$1,d0
	beq.s	loc_037830
	move.w	#$FFFF,d0
	bra.s	loc_037830
loc_03781A:
	move.w	$1C(a2),d0
	subq.w	#$1,$20(a2)
	bpl.s	loc_037830
	move.w	$22(a2),$20(a2)
	ror.w	#$1,d0
	move.w	d0,$1C(a2)
loc_037830:
	move.w	d0,$1E(a2)
	move.l	#$10000,d5
	divu.w	(a2),d5
	move.l	$18(a3),d6
	asr.l	#$8,d6
	muls.w	d5,d6
	add.l	d6,$14(a2)
	move.l	$14(a2),d6
	move.l	(a2),d0
	move.l	d0,d1
	move.l	$8(a2),d2
	add.l	d2,d1
	asr.l	#$1,d2
	sub.l	d2,d0
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	asr.w	#$1,d2
	asr.w	#$1,d3
	move.w	d2,d4
	move.w	d3,d5
	neg.w	d2
	neg.w	d3
	lea.l	$4(a4),a0
	move.w	(a0)+,d0
	beq.s	loc_03787E
	movea.w	d0,a0
	movem.w	d2-d5,$8(a0)
loc_03787E:
	lea.l	$C(a4),a0
	move.w	(a0)+,d0
	beq.s	loc_03789E
	movea.w	d0,a0
	movem.w	d2-d5,$8(a0)
	move.w	$22(a4),d0
	andi.w	#$4,d0
	bne.s	loc_03789E
	move.b	#$2,$1(a4)
loc_03789E:
	move.l	$3E(a6),d4
	move.l	$42(a6),d5
	move.l	$4(a2),d2
	move.l	d1,$4(a2)
	move.l	d2,d3
	sub.l	d1,d2
	asr.l	#$1,d2
	swap	d1
	move.w	d1,d0
	move.w	$54(a3),d3
	andi.w	#$B,d3
	beq.s	loc_0378E0
	move.l	d5,d3
	swap	d3
	add.w	$4A(a3),d3
	add.l	d2,d5
	move.l	d5,$42(a6)
	swap	d5
	sub.w	d5,d3
	swap	d5
	move.w	d3,$42(a3)
	move.w	d3,$4A(a3)
	bra.s	loc_0378F8
loc_0378E0:
	asr.w	#$1,d1
	addi.w	#$A,d1
	move.w	$42(a3),d2
	move.w	d1,$42(a3)
	cmp.w	d2,d1
	ble.s	loc_0378F4
	move.w	d2,d1
loc_0378F4:
	move.w	d1,$4A(a3)
loc_0378F8:
	move.l	$18(a2),d1
	move.l	d0,d2
	move.l	d2,d3
	swap	d2
	move.l	d6,d7
	sub.l	d1,d7
	swap	d7
	andi.w	#$3FE,d7
	lea.l	(Tbl_059B22).l,a0
	adda.w	d7,a0
	muls.w	$100(a0),d2
	muls.w	(a0),d3
	move.w	$24(a4),d7
	lea.l	$24(a2),a1
	bra.w	loc_0379B2
loc_037926:
	movem.l	d0-d1/d4-d7,-(a7)
	move.w	d0,d1
	swap	d0
	swap	d6
	andi.w	#$3FE,d6
	lea.l	(Tbl_059B22).l,a0
	adda.w	d6,a0
	muls.w	$100(a0),d0
	muls.w	(a0),d1
	movem.l	d0-d1,-(a7)
	add.l	d4,d4
	add.l	d5,d5
	add.l	d0,d4
	add.l	d1,d5
	add.l	d2,d4
	add.l	d3,d5
	asr.l	#$1,d4
	asr.l	#$1,d5
	sub.l	d2,d0
	sub.l	d3,d1
	asr.l	#$8,d0
	asr.l	#$8,d1
	jsr	(Mot_Atan2).l
	addi.w	#$200,d2
	andi.w	#$3FE,d2
	mulu.w	#$F00,d2
	swap	d2
	add.w	d2,d2
	add.w	d2,d2
	lea.l	Tbl_036DA8(pc),a0
	rol.w	$1E(a2)
	bcc.s	loc_037984
	lea.l	Tbl_ToyFigureChargedFrames(pc),a0
loc_037984:
	adda.w	d2,a0
	move.w	(a0)+,d1
	move.b	$38(a5),d6
	and.b	(a0)+,d6
	move.b	(a0)+,d0
	eor.b	d0,d6
	lsl.w	#$8,d6
	move.w	(a1),d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(a1)+
	swap	d4
	swap	d5
	jsr	(Spr_AddFront).l
	movem.l	(a7)+,d2-d3
	movem.l	(a7)+,d0-d1/d4-d7
	add.l	d1,d6
loc_0379B2:
	dbra	d7,loc_037926
	movem.l	(a7)+,a2-a6
	rts
