; ---------------------------------------------------------------------------
; ClockworkLogo_ScrollPicture - puts the Clockwork Tortoise picture (plane A) at the logo object's y: V-scroll A = -y
; In:  a6 = the logo object
; Out: g_VScrollA
; Evidence: Op2_CallNativeRel $2C from $49E6A every frame; idle f3678-f35341 (2250 frames over the attract loops), scenario-ending f142121-f142370
; ---------------------------------------------------------------------------
ClockworkLogo_ScrollPicture:
	move.w	$42(a6),d0
	neg.w	d0
	move.w	d0,(g_VScrollA).w
	rts
; ---------------------------------------------------------------------------
; ClockworkLogo_CycleColours - steps the logo's colour cycle: script variable 0 advances 2 bytes (wrapping at $6E) and 11 colours from Tbl_049EDC + it are copied into palette line 3 colours 5-15
; In:  a4 = the logo object's a4 block (var 0 = cycle offset)
; Out: g_PalSource3 colours 5-15; g_Pal3Reload set
; Evidence: a0 = Tbl_049EDC + $22(a4), a1 = g_PalSource3+$A, d7 = $A; Op2_CallNativeRel $3C from $49E66 after the timer-3 gate; runtime entry (ctarget) in idle
; ---------------------------------------------------------------------------
ClockworkLogo_CycleColours:
	lea.l	(Tbl_049EDC).l,a0
	lea.l	(g_PalSource3+$A).l,a1
	move.w	$22(a4),d0
	addq.w	#$2,d0
	cmpi.w	#$6E,d0
	blt.s	loc_049EBE
	moveq	#$0,d0
loc_049EBE:
	move.w	d0,$22(a4)
	adda.w	d0,a0
	moveq	#$A,d7
loc_049EC6:
	move.w	(a0)+,(a1)+
	dbra	d7,loc_049EC6
	st.b	(g_Pal3Reload).w
	rts
