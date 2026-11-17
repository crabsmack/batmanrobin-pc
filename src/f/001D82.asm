; ---------------------------------------------------------------------------
; Mot_JumpGravityD1 - computes the gravity g for a jump of height h in time T: with tick t = S/16 ($2E(a4)/16), g = 2h * 65536 * t^2 / (T * (T + t)), so decelerating from g*T/t to 0 over T/t runs covers h pixels
; In:  d1 = height h (word, pixels); a2 = script: word time T; a4 = speed block (speed factor $2E, tick squared $32)
; Out: d1 = gravity (long); a2 past the time word; d0, d2, d3 clobbered
; Evidence: the formula from the code (two scalings for T + t above or below $800); Mot_JumpLaunchSpeed turns it into the launch speed; executed in 137 corpus runs
; ---------------------------------------------------------------------------
Mot_JumpGravityD1:
	moveq	#$0,d0
	move.w	$2E(a4),d0
	swap	d1
	move.w	(a2)+,d2
	asr.l	#$4,d0
	move.w	d2,d3
	add.w	d0,d2
	cmpi.w	#$800,d2
	bgt.s	loc_001DAA
	muls.w	d3,d2
	asr.l	#$4,d1
	lsl.l	#$8,d2
	swap	d2
	divs.w	d2,d1
	muls.w	$32(a4),d1
	asr.l	#$3,d1
	rts
loc_001DAA:
	muls.w	d3,d2
	asr.l	#$6,d1
	lsl.l	#$6,d2
	swap	d2
	divs.w	d2,d1
	muls.w	$32(a4),d1
	asr.l	#$3,d1
	rts
