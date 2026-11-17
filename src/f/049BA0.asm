; ---------------------------------------------------------------------------
; PlayerSelect_SlideBatmanHalf - player-select native for the top (Batman) half, position = the chooser's x: when Robin is chosen it cruises right until x >= $118 (off screen); when Batman is chosen and the half is off screen it restarts at x -255 with cruise speed and brakes to rest at 0
; In:  a6 = the chooser, a3 = its motion block (+$18 x velocity, +$24 target speed, +$30 cruise)
; Out: a3 velocity / target speed, $3E(a6)
; Evidence: Op2_CallNativeRel $58 from $49B46 every frame; cmpi.w #$118 / #$FF01 against $3E(a6), tst.b g_P1IsRobin; TAS f431-f476 (Batman slides in), scenario-robin f485-f614
; ---------------------------------------------------------------------------
PlayerSelect_SlideBatmanHalf:
	tst.l	$24(a3)
	beq.s	loc_049BB6
	cmpi.w	#$118,$3E(a6)
	blt.s	loc_049BCA
	clr.l	$18(a3)
	clr.l	$24(a3)
loc_049BB6:
	tst.b	(g_P1IsRobin).w
	beq.s	loc_049BCC
	cmpi.w	#$118,$3E(a6)
	bge.s	loc_049BCA
	move.l	$30(a3),$24(a3)
loc_049BCA:
	rts
loc_049BCC:
	cmpi.w	#$FF01,$3E(a6)
	ble.s	loc_049BDC
	cmpi.w	#$118,$3E(a6)
	blt.s	loc_049BCA
loc_049BDC:
	move.w	#$FF01,$3E(a6)
	move.l	$30(a3),$18(a3)
	clr.l	$24(a3)
	rts
; ---------------------------------------------------------------------------
; PlayerSelect_SlideRobinHalf - player-select native for the bottom (Robin) half, position = the chooser's y word: when Batman is chosen it cruises left until y <= -280 (off screen); when Robin is chosen and the half is off screen it restarts at y $FF with -cruise speed and brakes to rest at 0
; In:  a6 = the chooser, a3 = its motion block (+$1C y velocity, +$2C target speed, +$34 cruise)
; Out: a3 velocity / target speed, $42(a6)
; Evidence: Op2_CallNativeRel $A2 from $49B4A every frame; cmpi.w #$FEE8 against $42(a6), neg.l of the cruise speed; mirrors PlayerSelect_SlideBatmanHalf; runtime entry (ctarget) in the TAS and scenario-robin
; ---------------------------------------------------------------------------
PlayerSelect_SlideRobinHalf:
	tst.l	$2C(a3)
	beq.s	loc_049C04
	cmpi.w	#$FEE8,$42(a6)
	bgt.s	loc_049C1C
	clr.l	$1C(a3)
	clr.l	$2C(a3)
loc_049C04:
	tst.b	(g_P1IsRobin).w
	bne.s	loc_049C1E
	cmpi.w	#$FEE8,$42(a6)
	ble.s	loc_049C1C
	move.l	$34(a3),$2C(a3)
	neg.l	$2C(a3)
loc_049C1C:
	rts
loc_049C1E:
	cmpi.w	#$FEE8,$42(a6)
	bgt.s	loc_049C1C
	move.w	#$FF,$42(a6)
	move.l	$34(a3),$1C(a3)
	neg.l	$1C(a3)
	clr.l	$2C(a3)
	rts
