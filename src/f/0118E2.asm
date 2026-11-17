; ---------------------------------------------------------------------------
; HUD_StartSlideOut - starts a panel sliding out toward home x -/+ $68 (off its screen edge) with a speed and acceleration scaled to the remaining distance
; In:  a6 = player record (+$A home x, +$E.l current x, +$25 bit 2 right side)
; Out: +$16 acceleration, +$12 = 4 x acceleration, +$25 bit 6 set (HUD_StepPanelSlide then accelerates it)
; Evidence: only caller HudOp_SlideOut (panel script op $C); ran in scenario-continue/game-over (frame 41744) and scenario-two-player (1249) when a panel switches to text
; ---------------------------------------------------------------------------
HUD_StartSlideOut:
	moveq	#$0,d1
	move.w	$A(a6),d1
	move.w	#$68,d0
	btst	#$2,$25(a6)
	bne.s	loc_0118F6
	neg.w	d0
loc_0118F6:
	add.w	d0,d1
	swap	d1
	sub.l	$E(a6),d1
	asr.l	#$2,d1
	divs.w	#$1D1,d1
	ext.l	d1
	asl.l	#$2,d1
	move.l	d1,$16(a6)
	add.l	d1,d1
	add.l	d1,d1
	move.l	d1,$12(a6)
	bset	#$6,$25(a6)
	rts
