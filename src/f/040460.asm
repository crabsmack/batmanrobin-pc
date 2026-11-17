; ---------------------------------------------------------------------------
; Stage3_CardHallFloorScroll - per-frame line H-scroll of the card hall's checkerboard floor on plane A: from screen line 104 - view y, 80 lines of a perspective ramp of the view x (x 1.3125, wrapped to one $60-px checker period, step -1/159 per line toward the horizon), then 40 lines continuing it with a bend from g_Shake2Pos (step = pos/256, growing by 1/16 of it per line)
; In:  -
; Out: g_HScrollLinesA lines 104 - view y .. +119
; Evidence: bsr from Stage3_RunCardHall's frame loop; Mem_FillRamp with d7 = $50, then a $28-line loop; divu #$6000 / #$9F; TAS f108331-f113271 (4622 frames); png f108600: the floor in perspective
; ---------------------------------------------------------------------------
Stage3_CardHallFloorScroll:
	move.l	(g_ViewX).w,d0
	move.l	d0,d1
	lsr.l	#$2,d1
	add.l	d1,d0
	lsr.l	#$2,d1
	add.l	d1,d0
	lsr.l	#$8,d0
	divu.w	#$6000,d0
	move.w	d0,d2
	clr.w	d0
	lsr.l	#$8,d0
	cmpi.l	#$600000,d0
	blt.s	loc_040488
	subi.l	#$600000,d0
loc_040488:
	move.l	d0,d1
	divu.w	#$9F,d1
	andi.l	#$FFFF,d1
	addi.l	#$8000,d0
	neg.l	d0
	lea.l	(g_HScrollLinesA+$D0).l,a0
	move.w	(g_ViewY).w,d7
	add.w	d7,d7
	suba.w	d7,a0
	move.w	#$4F,d7
	mulu.w	d1,d7
	add.l	d7,d0
	neg.l	d1
	moveq	#$50,d7
	jsr	(Mem_FillRamp).l
	sub.l	d1,d0
	lsr.w	#$1,d2
	bcc.s	loc_0404C8
	subi.l	#$600000,d0
loc_0404C8:
	move.l	(g_Shake2Pos).w,d1
	asr.l	#$8,d1
	move.l	d1,d2
	asr.l	#$4,d2
	moveq	#$27,d7
loc_0404D4:
	swap	d0
	move.w	d0,(a0)+
	swap	d0
	add.l	d1,d0
	add.l	d2,d1
	dbra	d7,loc_0404D4
	rts
