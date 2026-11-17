; ---------------------------------------------------------------------------
; Mot_SetTarget - stores a target point in the motion block ($14/$16(a3)) and classifies the direction to it: bit 3 of (a3) set when |dy| >= |dx| (vertical-dominant), bit 4 set when the dominant component is negative
; In:  d0 = target x, d1 = target y; a3 = motion block, a6 = object (x $3E, y $42)
; Out: d0 = dx, d1 = dy (target - position), d2 = |dx|, d3 = |dy|
; Evidence: callers the aim ops $1364/$1382/$13A4, which pass d0/d1 on to Mot_Atan2 (atan2); Op2_BranchPastTarget reads bits 3/4; 765 TAS frames
; ---------------------------------------------------------------------------
Mot_SetTarget:
	move.w	d0,$14(a3)
	move.w	d1,$16(a3)
	sub.w	$3E(a6),d0
	move.w	d0,d2
	bpl.s	loc_0013DC
	neg.w	d2
loc_0013DC:
	sub.w	$42(a6),d1
	move.w	d1,d3
	bpl.s	loc_0013E6
	neg.w	d3
loc_0013E6:
	cmp.w	d2,d3
	blt.s	loc_0013F8
	bset	#$3,(a3)
	tst.w	d1
	bmi.s	loc_001400
loc_0013F2:
	bclr	#$4,(a3)
	rts
loc_0013F8:
	bclr	#$3,(a3)
	tst.w	d0
	bpl.s	loc_0013F2
loc_001400:
	bset	#$4,(a3)
	rts
