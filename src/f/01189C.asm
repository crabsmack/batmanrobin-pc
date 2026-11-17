; ---------------------------------------------------------------------------
; HUD_StartSlideIn - starts a panel sliding in: puts it $68 px off its screen edge with speed 31 x the braking step, so it stops exactly at home x
; In:  a6 = player record (+$A home x, +$25 bit 2 right side)
; Out: +$E start x, +$12 speed, +$16 braking step (both 16.16), +$25 bit 6 cleared
; Evidence: only caller HudOp_SlideIn (panel script op $10); ran 16 times in the TAS (532-128791) when panels switch content; the 104 px distance is divided by $1D1 = 465 = 1+2+...+30, the distance the braking speeds cover (inferred from the arithmetic)
; ---------------------------------------------------------------------------
HUD_StartSlideIn:
	moveq	#$0,d4
	move.w	$A(a6),d4
	move.l	d4,d5
	move.w	#$68,d0
	btst	#$2,$25(a6)
	bne.s	loc_0118B2
	neg.w	d0
loc_0118B2:
	add.w	d0,d5
	swap	d4
	swap	d5
	sub.l	d5,d4
	asr.l	#$2,d4
	divs.w	#$1D1,d4
	move.w	d4,d0
	muls.w	#$1F,d0
	ext.l	d4
	asl.l	#$2,d4
	asl.l	#$2,d0
	move.l	d0,$12(a6)
	move.l	d4,$16(a6)
	sub.l	d4,d5
	move.l	d5,$E(a6)
	bclr	#$6,$25(a6)
	rts
