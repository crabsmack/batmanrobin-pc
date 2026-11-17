; ---------------------------------------------------------------------------
; Story_MonitorsAlarmScroll - monitors object's slot-2 native once the alarm goes: red tint of all four palette lines toggles 0/+4 every 8 frames ($2A(a6) counter), both planes V-scroll = y + shake
; In:  a6 = object
; Out: g_PalNTintR, g_VScrollA/B
; Evidence: lsr #2 / andi #2 / add.b on the counter; set as coroutine slot 2 by Op2_StartCoroutine2Rel at $0488DA, called by Op2_CallSlot2Native; scenario-ending 140822-145128
; ---------------------------------------------------------------------------
Story_MonitorsAlarmScroll:
	move.w	$2A(a6),d0
	addq.w	#$1,d0
	move.w	d0,$2A(a6)
	lsr.w	#$2,d0
	andi.w	#$2,d0
	add.b	d0,d0
	move.b	d0,(g_Pal0TintR).w
	move.b	d0,(g_Pal2TintR).w
	move.b	d0,(g_Pal3TintR).w
	move.b	d0,(g_Pal1TintR).w
	move.w	$42(a6),d0
	add.w	(g_ShakeY).w,d0
	move.w	d0,(g_VScrollA).w
	move.w	d0,(g_VScrollB).w
	rts
; ---------------------------------------------------------------------------
; Story_MonitorsScroll - monitors object's first slot-2 native: plane B V-scroll = y + shake, plane A $50 px less
; In:  a6 = object
; Out: g_VScrollA/B
; Evidence: set as coroutine slot 2 by Op2_StartCoroutine2Rel at $048890 (start of Scr2_048884), called by Op2_CallSlot2Native at $04889C; runtime entry in the ctarget union
; ---------------------------------------------------------------------------
Story_MonitorsScroll:
	move.w	$42(a6),d0
	add.w	(g_ShakeY).w,d0
	move.w	d0,(g_VScrollB).w
	subi.w	#$50,d0
	move.w	d0,(g_VScrollA).w
	rts
