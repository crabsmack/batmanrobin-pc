; ---------------------------------------------------------------------------
; Stage4_TubeScroll - the ring tunnel's scroll per frame: speed g_SceneVars+4 eases toward g_SceneVars (-$800 per frame, clamped -2..-12 px), position +8 wraps within one 64-px ring; the line pairs get plane A/B x = position +/- an offset that grows from the screen edges and from the centre line 112 toward each other
; In:  g_SceneVars (wanted speed, 16.16)
; Out: g_SceneVars+4/+8, the interleaved line buffer $FFE1D2-$FFE551 (mode $10)
; Evidence: a2 = line 0 and a5 = line 223 (from the address of g_HIntTblFront = the buffer end), a3/a4 = line 112; called every frame by Stage4_RunTube, TAS 122681-128749
; ---------------------------------------------------------------------------
Stage4_TubeScroll:
	move.l	(g_SceneVars+$8).w,d2
	move.l	(g_SceneVars).w,d1
	move.l	(g_SceneVars+$4).w,d0
	cmp.l	d0,d1
	blt.s	loc_047A86
	move.l	d1,d0
	bra.s	loc_047A8C
loc_047A86:
	addi.l	#$FFFFF800,d0
loc_047A8C:
	cmpi.l	#$FFFE0000,d0
	ble.s	loc_047A9A
	move.l	#$FFFE0000,d0
loc_047A9A:
	cmpi.l	#$FFF40000,d0
	bge.s	loc_047AA8
	move.l	#$FFF40000,d0
loc_047AA8:
	move.l	d0,(g_SceneVars+$4).w
	add.l	d0,d2
	cmpi.l	#$0,d2
	blt.s	loc_047ABC
	subi.l	#$400000,d2
loc_047ABC:
	cmpi.l	#$FFC00000,d2
	bge.s	loc_047ACA
	addi.l	#$400000,d2
loc_047ACA:
	move.l	d2,(g_SceneVars+$8).w
	lea.l	(g_HScrollLinesA).l,a2
	lea.l	(g_HScrollLinesB).l,a3
	lea.l	(g_HScrollLinesB).l,a4
	lea.l	(g_HIntTblFront).l,a5
	moveq	#$0,d0
	move.l	(g_SceneVars+$8).l,d5
	divs.w	#$E0,d5
	ext.l	d5
	move.l	d5,d1
	muls.w	#$70,d1
	moveq	#$0,d2
	moveq	#-$2,d3
	move.w	#$1BE,d4
	bra.s	loc_047B34
loc_047B04:
	move.l	(g_SceneVars+$8).l,d6
	move.l	d6,d7
	add.l	d0,d6
	sub.l	d0,d7
	swap	d6
	swap	d7
	move.w	d6,(a2)+
	move.w	d7,(a2)+
	move.w	d7,-(a5)
	move.w	d6,-(a5)
loc_047B1C:
	move.l	(g_SceneVars+$8).l,d6
	move.l	d6,d7
	add.l	d1,d6
	sub.l	d1,d7
	swap	d6
	swap	d7
	move.w	d6,(a3)+
	move.w	d7,(a3)+
	move.w	d7,-(a4)
	move.w	d6,-(a4)
loc_047B34:
	add.l	d5,d0
	addq.w	#$4,d3
	sub.w	d3,d2
	bpl.s	loc_047B1C
	add.w	d4,d2
	subq.w	#$4,d4
	sub.l	d5,d1
	cmp.l	d0,d1
	ble.s	loc_047B04
	rts
