; ---------------------------------------------------------------------------
; Stage1_BalloonBob - balloon-bomb native: cruises the balloon vertically toward the screen height $2A(a6) and, once within 4 px, swaps it with $2C(a6), so it bobs between the two heights
; In:  a3 = motion block (+$34 cruise speed y), a6 = balloon (+$42 y, +$2A/$2C screen-y heights)
; Out: $2C(a3) target y speed = -/+ cruise, $2A/$2C possibly swapped
; Evidence: y - g_CameraY compared with $2A(a6), exg d1,d2 within 4 px; called every frame by Scr2_0203E0 objects (TAS f13768-f18211, 3270 frames); RAM f14400 four balloons at screen y 100-200 bobbing (png f014400: blue balloons carrying spiked bombs)
; ---------------------------------------------------------------------------
Stage1_BalloonBob:
	move.l	$34(a3),d0
	move.w	$2A(a6),d1
	move.w	$2C(a6),d2
	move.w	$42(a6),d3
	sub.w	(g_CameraY).l,d3
	sub.w	d1,d3
	blt.s	loc_0203CC
	neg.l	d0
	subq.w	#$4,d3
	bpl.s	loc_0203D2
	bra.s	loc_0203D0
loc_0203CC:
	addq.w	#$4,d3
	bmi.s	loc_0203D2
loc_0203D0:
	exg.l	d1,d2
loc_0203D2:
	move.l	d0,$2C(a3)
	move.w	d1,$2A(a6)
	move.w	d2,$2C(a6)
	rts
