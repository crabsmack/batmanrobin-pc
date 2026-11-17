; ---------------------------------------------------------------------------
; Stage4_LabFloorLines - plane A H-scroll of the lab floor: lines 215 up to 136 spread by a per-line step from the segment offset (perspective), lines 135-120 one value ($30 px further on odd segments)
; In:  g_SceneVars+4 (offset), g_SceneVars+8 (segment), g_ShakeY
; Out: g_HScrollLinesA lines 120-215 (written backward from +$1B0 - 2*shake); d0 = the last value (16.16, swapped)
; Evidence: 80 + 16 words written downward from $FFE382; andi #1 on g_SceneVars+8; called by Stage4_LabScroll
; ---------------------------------------------------------------------------
Stage4_LabFloorLines:
	lea.l	(g_HScrollLinesA+$1B0).l,a2
	move.w	(g_ShakeY).w,d0
	add.w	d0,d0
	suba.w	d0,a2
	move.l	(g_SceneVars+$4).w,d0
	move.l	d0,d1
	divs.w	#$160,d1
	ext.l	d1
	asl.l	#$1,d1
	addi.l	#$FEE08000,d0
	moveq	#$4F,d7
loc_047866:
	swap	d0
	move.w	d0,-(a2)
	swap	d0
	sub.l	d1,d0
	dbra	d7,loc_047866
	andi.w	#$1,(g_SceneVars+$8).w
	beq.s	loc_047880
	subi.l	#$300000,d0
loc_047880:
	moveq	#$F,d7
	swap	d0
loc_047884:
	move.w	d0,-(a2)
	dbra	d7,loc_047884
	rts
