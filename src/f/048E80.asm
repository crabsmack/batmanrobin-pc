; ---------------------------------------------------------------------------
; Attract_RooftopLightning - native of the rooftop object: line tint (Tint_Step) on line 3 and its negative on lines 0-2, and while sprite var 0 ($22(a5)) counts down, palette line 3 colour 1 = a grey ramp from it (the lightning flash)
; In:  a6/a5 = object and its sprite block
; Out: g_Pal0-3Tint, g_PalSource+$62, g_Pal3Reload
; Evidence: (v >> 1) & $E replicated into the three nibbles -> $E134; only caller Op2_CallNative at $048D02 in Scr2_048CE2; idle 926-1160 + 3925n, scenario-ending 139353-143520
; ---------------------------------------------------------------------------
Attract_RooftopLightning:
	jsr	(Tint_Step).l
	move.b	d0,(g_Pal3TintR).w
	move.b	d0,(g_Pal3TintG).w
	move.b	d0,(g_Pal3TintB).w
	neg.b	d0
	move.b	d0,(g_Pal0TintR).w
	move.b	d0,(g_Pal0TintG).w
	move.b	d0,(g_Pal0TintB).w
	move.b	d0,(g_Pal1TintR).w
	move.b	d0,(g_Pal1TintG).w
	move.b	d0,(g_Pal1TintB).w
	move.b	d0,(g_Pal2TintR).w
	move.b	d0,(g_Pal2TintG).w
	move.b	d0,(g_Pal2TintB).w
	move.w	$22(a5),d0
	subq.w	#$1,d0
	bmi.s	loc_048EDC
	move.w	d0,$22(a5)
	asr.w	#$1,d0
	andi.w	#$E,d0
	move.w	d0,d1
	lsl.w	#$4,d0
	or.w	d1,d0
	lsl.w	#$4,d0
	or.w	d1,d0
	move.w	d0,(g_PalSource+$62).w
	st.b	(g_Pal3Reload).w
loc_048EDC:
	rts
