; ---------------------------------------------------------------------------
; Stage4_LabScroll - lab scroll per frame: V-scroll of both planes = g_ViewY; the floor x = g_ViewX * 35/32 split into an 87-px segment number (g_SceneVars+8) and the offset within it (+4); then the floor lines, the upper-wall lines (offset +$A from the floor's last line, wrapped) and the plane B lines
; In:  none
; Out: g_VScrollA/B, g_SceneVars+0/+4/+8/+$A, the line buffers
; Evidence: divu #$5700 on (x >> 8); calls Stage4_LabFloorLines, Stage4_LabUpperLines, then Stage4_LabPlaneBLines; every frame of TAS 119416-122651
; ---------------------------------------------------------------------------
Stage4_LabScroll:
	move.w	(g_ViewY).w,d0
	move.w	d0,(g_VScrollA).w
	move.w	d0,(g_VScrollB).w
	move.l	(g_ViewX).w,d0
	move.l	d0,d1
	lsr.l	#$3,d1
	add.l	d1,d0
	lsr.l	#$2,d1
	sub.l	d1,d0
	move.l	d0,(g_SceneVars).w
	lsr.l	#$8,d0
	divu.w	#$5700,d0
	move.w	d0,(g_SceneVars+$8).l
	clr.w	d0
	lsr.l	#$8,d0
	neg.l	d0
	move.l	d0,(g_SceneVars+$4).w
	bsr.w	Stage4_LabFloorLines
	swap	d0
	asr.l	#$8,d0
	muls.w	#$180,d0
	subi.l	#$5F0000,d0
	cmpi.l	#$FFE00000,d0
	blt.s	loc_047828
	subi.l	#$8F0000,d0
loc_047828:
	cmpi.l	#$FF500000,d0
	bgt.s	loc_047836
	addi.l	#$8F0000,d0
loc_047836:
	move.l	d0,(g_SceneVars+$A).w
	bsr.w	Stage4_LabUpperLines
	bra.w	Stage4_LabPlaneBLines
