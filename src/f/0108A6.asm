; ---------------------------------------------------------------------------
; Hitbox_OverlapCenter - returns the centre of the intersection of box a2's rectangle (+$12..+$18) and the rectangle d4-d7
; In:  a2 = box, d4 = x1, d5 = y1, d6 = x2, d7 = y2
; Out: d4 = centre x, d5 = centre y (d0-d3 = the box's x1, y1, x2, y2)
; Evidence: only caller Hitbox_SpawnAtContact; movem.w $12(a2),d0-d3 then max of the lows, min of the highs, averaged
; ---------------------------------------------------------------------------
Hitbox_OverlapCenter:
	movem.w	$12(a2),d0-d3
	cmp.w	d4,d0
	bmi.s	loc_0108B2
	move.w	d0,d4
loc_0108B2:
	cmp.w	d2,d6
	bmi.s	loc_0108B8
	move.w	d2,d6
loc_0108B8:
	cmp.w	d5,d1
	bmi.s	loc_0108BE
	move.w	d1,d5
loc_0108BE:
	cmp.w	d3,d7
	bmi.s	loc_0108C4
	move.w	d3,d7
loc_0108C4:
	add.w	d6,d4
	asr.w	#$1,d4
	add.w	d7,d5
	asr.w	#$1,d5
	rts
