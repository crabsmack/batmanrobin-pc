; ---------------------------------------------------------------------------
; Stage4_LabUpperLines - plane A H-scroll of the lab's upper wall: lines from 24 down and from 119 up meet in the middle, each pair a perspective value from g_SceneVars+$A (incremental quadratic stepping)
; In:  g_SceneVars+$A, g_ShakeY
; Out: g_HScrollLinesA lines 24-119
; Evidence: a2 = $FFE202 forward, a3 = $FFE2C2 backward, d2/d3/d4 step loop; called by Stage4_LabScroll
; ---------------------------------------------------------------------------
Stage4_LabUpperLines:
	lea.l	(g_HScrollLinesA+$30).l,a2
	lea.l	(g_HScrollLinesA+$F0).l,a3
	move.w	(g_ShakeY).w,d0
	add.w	d0,d0
	suba.w	d0,a2
	suba.w	d0,a3
	move.l	#$8000,d0
	move.l	(g_SceneVars+$A).w,d5
	move.l	d5,d7
	subi.l	#$1200000,d7
	neg.l	d5
	divu.w	#$120,d5
	move.w	d5,d1
	mulu.w	#$5F,d1
	neg.l	d1
	andi.l	#$FFFF,d5
	neg.l	d5
	moveq	#$0,d2
	moveq	#-$2,d3
	move.w	#$17E,d4
loc_0478D2:
	move.l	d7,d6
	sub.l	d0,d6
	swap	d6
	move.w	d6,(a2)+
loc_0478DA:
	move.l	d7,d6
	sub.l	d1,d6
	swap	d6
	move.w	d6,-(a3)
	add.l	d5,d0
	addq.w	#$4,d3
	sub.w	d3,d2
	bpl.s	loc_0478DA
	add.w	d4,d2
	subq.w	#$4,d4
	sub.l	d5,d1
	cmp.l	d0,d1
	ble.s	loc_0478D2
	rts
; ---------------------------------------------------------------------------
; Stage4_LabPlaneBLines - plane B H-scroll of the lab: lines 32-119 = a sine wobble (/64, phase g_SceneVars+$E += $A per frame, +$10 per line) minus g_SceneVars * 0.4, then 48 lines from 176 (plus shake) at -1.25 * g_CameraX (the foreground)
; In:  g_SceneVars, g_SceneVars+$E, g_CameraX, g_ShakeY
; Out: g_HScrollLinesB lines 32-119 and 176-223
; Evidence: a1 = $FFE3D2 (88 words), then $FFE4F2 (48 + shake words); reached by bra from Stage4_LabScroll
; ---------------------------------------------------------------------------
Stage4_LabPlaneBLines:
	lea.l	(Tbl_059B22).l,a0
	lea.l	(g_HScrollLinesB+$40).l,a1
	addi.w	#$A,(g_SceneVars+$E).w
	move.w	(g_SceneVars+$E).w,d1
	move.w	(g_SceneVars).w,d2
	muls.w	#$6580,d2
	swap	d2
	move.w	#$57,d7
loc_04791A:
	andi.w	#$3FE,d1
	move.b	(a0,d1.w),d0
	ext.w	d0
	asr.w	#$6,d0
	sub.w	d2,d0
	move.w	d0,(a1)+
	addi.w	#$10,d1
	dbra	d7,loc_04791A
	lea.l	(g_HScrollLinesB+$160).l,a1
	moveq	#$2F,d7
	move.w	(g_ShakeY).w,d0
	add.w	d0,d7
	add.w	d0,d0
	suba.w	d0,a1
	move.l	(g_CameraX).w,d0
	move.l	d0,d1
	asr.l	#$2,d1
	add.l	d1,d0
	swap	d0
	neg.w	d0
loc_047952:
	move.w	d0,(a1)+
	dbra	d7,loc_047952
	rts
