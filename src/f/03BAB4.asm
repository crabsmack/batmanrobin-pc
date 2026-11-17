; ---------------------------------------------------------------------------
; Boss_CatHead_DriverSettle - moves the raster driver's x and y ($3E/$42(a6), 16.16) toward 0 by its accelerations ($20/$28(a3)), so the face's sway and bulge die away; d0 = 1 while either is still nonzero
; In:  a6 = the driver, a3 = its motion block
; Out: $3E/$42(a6); d0 = 0 settled / 1 not yet (Op2_CallNativeRel ignores it)
; Evidence: Op2_CallNativeRel at $03BAAE after Op2_SetAccelForDist $D5/$30, every frame of a coroutine; TAS f89343-f89520 (178 frames, right after the head's energy reached 0)
; ---------------------------------------------------------------------------
Boss_CatHead_DriverSettle:
	move.l	$20(a3),d0
	move.l	$3E(a6),d4
	bpl.s	loc_03BAC4
	add.l	d0,d4
	bmi.s	loc_03BACA
	bra.s	loc_03BAC8
loc_03BAC4:
	sub.l	d0,d4
	bpl.s	loc_03BACA
loc_03BAC8:
	moveq	#$0,d4
loc_03BACA:
	move.l	d4,$3E(a6)
	move.l	$28(a3),d0
	move.l	$42(a6),d5
	bpl.s	loc_03BADE
	add.l	d0,d5
	bmi.s	loc_03BAE4
	bra.s	loc_03BAE2
loc_03BADE:
	sub.l	d0,d5
	bpl.s	loc_03BAE4
loc_03BAE2:
	moveq	#$0,d5
loc_03BAE4:
	move.l	d5,$42(a6)
	move.l	d4,d0
	or.l	d5,d0
	beq.s	loc_03BAF0
	moveq	#$1,d0
loc_03BAF0:
	rts
