; ---------------------------------------------------------------------------
; Stage4_TreadFrameByX - returns (x / 2) mod 3 for the object a6: the 3-frame tread animation phase, so the treads turn as the object moves
; In:  a6 = object (+$3E x)
; Out: d0 = 0-2
; Evidence: Op2_CallNativeRelSwitch at $42E96 (vent tank animation: DrawSpr $4B32/$4B28/$4B1E) and $430D0 (gun tank animation $430C4: DrawSprOff $4BB4/$4BBE/$4BC8); divu #3 remainder; 1964 TAS frames 119574-122361 (stage 4 lab)
; ---------------------------------------------------------------------------
Stage4_TreadFrameByX:
	moveq	#$0,d0
	move.w	$3E(a6),d0
	lsr.w	#$1,d0
	divu.w	#$3,d0
	swap	d0
	rts
