; ---------------------------------------------------------------------------
; Stage4_HallFloorRipple - hall scroll per frame: plane B V-scroll = g_ViewY * 7/8 and the 64 plane-B lines 160-223 (the floor) shifted by -g_CameraX plus a sine ripple from two phases (+1 and +2 per frame, swapped on odd frames)
; In:  none
; Out: g_VScrollB, g_HScrollLinesB+$140..
; Evidence: phases g_SceneVars+4/+6 and +0, Tbl_059B22 sine >> 2; called every frame by Stage4_RunHall; TAS 116192-119385
; ---------------------------------------------------------------------------
Stage4_HallFloorRipple:
	move.w	(g_ViewY).w,d0
	asr.w	#$1,d0
	move.w	d0,d2
	asr.w	#$1,d0
	add.w	d0,d2
	asr.w	#$1,d0
	add.w	d0,d2
	move.w	d2,(g_VScrollB).w
	lea.l	(Tbl_059B22).l,a0
	lea.l	(g_HScrollLinesB+$140).l,a1
	addq.w	#$1,(g_SceneVars+$4).w
	addq.w	#$2,(g_SceneVars+$6).w
	move.w	(g_SceneVars+$4).w,d1
	move.w	(g_SceneVars+$6).w,d2
	addq.w	#$1,(g_SceneVars).w
	add.w	(g_SceneVars).w,d2
	move.w	(g_CameraX).w,d3
	move.w	#$E,d4
	move.w	#$3FE,d5
	move.w	#$1F,d7
	tst.b	(g_FrameParity).w
	beq.s	loc_0476C4
	exg.l	d1,d2
loc_0476C4:
	and.w	d5,d1
	move.b	(a0,d1.w),d0
	ext.w	d0
	asr.w	#$2,d0
	sub.w	d3,d0
	move.w	d0,(a1)+
	add.w	d4,d1
	and.w	d5,d2
	move.b	(a0,d2.w),d0
	ext.w	d0
	asr.w	#$2,d0
	sub.w	d3,d0
	move.w	d0,(a1)+
	add.w	d4,d2
	dbra	d7,loc_0476C4
	rts
