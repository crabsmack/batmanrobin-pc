; ---------------------------------------------------------------------------
; Boss_CatHead_SwayPoint - maps a point of the cat-head's face through the arena's sway (g_SceneVars+$10 shear about the pivot g_SceneVars+$12/$14): d4/d5 = the swayed x/y (16.16, integer in the high word after a swap)
; In:  d0/d1 = the point's x/y; g_SceneVars+$10 (shear), +$12/+$14 (pivot), +$16
; Out: d2.w = the shear, d3 = -(pivot y term x shear), d4/d5 = the swayed point (the integer in the low word)
; Evidence: the bsr.b at $03B7D2 from the eye-shot natives; moveq / ext.l / asl.l / muls.w of the shear with the pivot longs at g_SceneVars+$12/+$14 (as Boss_CatHead_SwayRaster's ramp about the pivot); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_CatHead_SwayPoint:
	moveq	#$0,d5
	move.w	(g_SceneVars+$10).w,d2
	move.w	d2,d4
	ext.l	d4
	asl.l	#$3,d4
	bpl.s	loc_03B776
	move.w	d2,d5
	ext.l	d5
	asl.l	#$5,d5
	neg.l	d5
loc_03B776:
	move.l	(g_SceneVars+$12).w,d3
	neg.l	d3
	swap	d3
	add.w	d0,d3
	muls.w	d2,d3
	add.l	d3,d5
	swap	d5
	add.w	d1,d5
	move.l	(g_SceneVars+$14).w,d3
	neg.l	d3
	swap	d3
	add.w	d1,d3
	muls.w	d2,d3
	neg.l	d3
	add.l	d3,d4
	swap	d4
	add.w	d0,d4
	rts
