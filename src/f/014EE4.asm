; ---------------------------------------------------------------------------
; Mot_Atan2_RightHalf - the dx > 0 half of Mot_Atan2: headings $300-$3FF and 0-$100 from the octant ratio
; In:  d0 = dx > 0, d1 = dy
; Out: d2 = heading
; Evidence: reached by bne from Mot_Atan2 after tst.w d0 / bmi; dy = 0 returns 0, dy < 0 builds $400 - ratio or $300 + ratio
; ---------------------------------------------------------------------------
Mot_Atan2_RightHalf:
	tst.w	d1
	bmi.s	loc_014F06
	beq.s	Mot_Atan2_Zero
	cmp.w	d1,d0
	bmi.s	loc_014EF8
	ext.l	d1
	lsl.l	#$7,d1
	divu.w	d0,d1
	move.w	d1,d2
	rts
loc_014EF8:
	ext.l	d0
	lsl.l	#$7,d0
	divu.w	d1,d0
	move.w	#$100,d2
	sub.w	d0,d2
	rts
loc_014F06:
	neg.w	d1
	cmp.w	d1,d0
	bmi.s	loc_014F1A
	ext.l	d1
	lsl.l	#$7,d1
	divu.w	d0,d1
	move.w	#$400,d2
	sub.w	d1,d2
	rts
loc_014F1A:
	ext.l	d0
	lsl.l	#$7,d0
	divu.w	d1,d0
	move.w	#$300,d2
	add.w	d0,d2
	rts
; ---------------------------------------------------------------------------
; Mot_Atan2_LeftHalf - the dx < 0 half of Mot_Atan2: headings $100-$300 from the octant ratio of |dx| and |dy|
; In:  d0 = dx < 0, d1 = dy
; Out: d2 = heading; d0/d1 negated to |dx|, |dy| (then the ratio's divu)
; Evidence: neg.w d0 / tst.w d1, then $200 - ratio, $100 + ratio, $200 + ratio or $300 - ratio; reached by bmi from Mot_Atan2
; ---------------------------------------------------------------------------
Mot_Atan2_LeftHalf:
	neg.w	d0
	tst.w	d1
	bmi.s	loc_014F56
	bne.s	loc_014F36
	move.w	#$200,d2
	rts
loc_014F36:
	cmp.w	d1,d0
	bmi.s	loc_014F48
	ext.l	d1
	lsl.l	#$7,d1
	divu.w	d0,d1
	move.w	#$200,d2
	sub.w	d1,d2
	rts
loc_014F48:
	ext.l	d0
	lsl.l	#$7,d0
	divu.w	d1,d0
	move.w	#$100,d2
	add.w	d0,d2
	rts
loc_014F56:
	neg.w	d1
	cmp.w	d1,d0
	bmi.s	loc_014F6A
	ext.l	d1
	lsl.l	#$7,d1
	divu.w	d0,d1
	move.w	#$200,d2
	add.w	d1,d2
	rts
loc_014F6A:
	ext.l	d0
	lsl.l	#$7,d0
	divu.w	d1,d0
	move.w	#$300,d2
	sub.w	d0,d2
	rts
