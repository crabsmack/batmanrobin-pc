; ---------------------------------------------------------------------------
; Boss_BeamShip_DeathFlash - native fragment in the death disc script: palette line 1 colour 15 = $222, Fade_FlashWhite7, Tint_FlashC3 (the big flash after the disc's 7 flashing pulses)
; In:  a6 = the radius object
; Out: continues the a2 script
; Evidence: Op2_Native at $29606 in Scr2_0295D4 after a loop of 7 Fade_FlashWhite3 + Shake_StartBig; TAS f79050
; ---------------------------------------------------------------------------
Boss_BeamShip_DeathFlash:
	move.w	#$222,(g_PalSource1+$1E).w
	jsr	(Fade_FlashWhite7).l
	jsr	(Tint_FlashC3).l
	jsr	($7FE).w
