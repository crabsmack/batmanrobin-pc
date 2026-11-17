; ---------------------------------------------------------------------------
; Stage1_UpdateStreetScroll - per-frame scroll of the stage 1 street: facade x = g_ViewX * 35/32, the current $168-px facade segment (shifting the sign slots when it advances), the plane B line scroll, then the plane A fire sway or sky wave
; In:  none
; Out: $F506 facade x (16.16), $F50A offset in the segment, $F50E segment index; g_HScrollLinesA/B filled
; Evidence: x + x/8 - x/32; divu #$B400 (= $168 * $80) with rounding at -$B4; on a new segment $F4FA.w = $F4FC.w, $F4FC = 0; $F518: 0 Stage1_WaveSkyLines, > 0 Stage1_SwayFireLines, < 0 (boss) nothing; called every frame by the street loop $1FB3A, TAS f532-f6334
; ---------------------------------------------------------------------------
Stage1_UpdateStreetScroll:
	move.l	(g_ViewX).w,d0
	move.l	d0,d1
	lsr.l	#$3,d1
	add.l	d1,d0
	lsr.l	#$2,d1
	sub.l	d1,d0
	move.l	d0,(g_SceneVars+$12).w
	lsr.l	#$8,d0
	lsr.l	#$1,d0
	divu.w	#$B400,d0
	move.w	d0,d1
	clr.w	d0
	lsr.l	#$7,d0
	neg.l	d0
	cmpi.l	#$FF4C0000,d0
	bgt.s	loc_015D66
	addq.w	#$1,d1
	addi.l	#$1680000,d0
loc_015D66:
	move.l	d0,(g_Stage1SegOfs).w
	cmp.w	(g_Stage1Segment).w,d1
	ble.s	loc_015D7E
	move.w	d1,(g_Stage1Segment).w
	move.w	(g_SceneVars+$8).w,(g_SceneVars+$6).w
	clr.w	(g_SceneVars+$8).w
loc_015D7E:
	bsr.w	Stage1_ScrollFacadeLines
	tst.b	(g_Stage1FireState).w
	beq.w	Stage1_WaveSkyLines
	bpl.w	Stage1_SwayFireLines
	rts
