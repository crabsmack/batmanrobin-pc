; ---------------------------------------------------------------------------
; Continue_ScrollPicture - continue screen per-frame native: puts plane A (Mr. Freeze's head) at the object's bobbing x/y plus the screen shake, plane B at half the shake, and flickers palette line 1's green tint (0 / -2 every frame) unless the fade is above normal
; In:  a6 = the continue-screen object (x $3E, y $42: oscillator Op2_SetOscY)
; Out: g_HScrollA, g_VScrollA, g_VScrollB, g_Pal1TintG
; Evidence: Op2_CallNativeRel $210 from $491E8 every frame; g_ShakeY added, asr #1 for plane B; eori.b #$FE,g_Pal1TintG while g_FadeLevelVInt <= 0; png f42600 (green countdown monitors); scenario-continue f41999-f42251, scenario-game-over f41999-f42806
; ---------------------------------------------------------------------------
Continue_ScrollPicture:
	move.w	$3E(a6),(g_HScrollA).w
	move.w	(g_ShakeY).w,d1
	move.w	$42(a6),d0
	add.w	d1,d0
	move.w	d0,(g_VScrollA).w
	asr.w	#$1,d1
	move.w	d1,(g_VScrollB).w
	tst.b	(g_FadeLevelVInt).w
	ble.s	loc_049420
	clr.b	(g_Pal1TintG).w
	rts
loc_049420:
	eori.b	#$FE,(g_Pal1TintG).w
	rts
