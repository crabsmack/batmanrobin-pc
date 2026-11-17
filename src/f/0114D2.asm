; ---------------------------------------------------------------------------
; HUD_DrawWeaponBar - draws the weapon charge bar under the score into the panel buffer: fill from the charge byte +$8 (max $2B, flashing when full), colour set by weapon type +$28, alternate cells while the timer +$26 runs (blinking below 150)
; In:  a6 = player record, a0 = bar tile-index map (Tbl_051792 P1 / Tbl_054812 P2), a1 = bar tiles (Tbl_052512 / Tbl_055592)
; Out: 6 columns x 2 tiles written at g_HUDTileBuf+$20 (stride $60)
; Evidence: called by HUD_StepP1Panel/HUD_StepP2Panel; TAS frames 3000/12000/30000: bar red-yellow for type 0, green for type 2, half empty with charge $0F; Weapon_Fire uses +$8 >= $2B00 for the charged shot and clears it
; ---------------------------------------------------------------------------
HUD_DrawWeaponBar:
	move.b	$8(a6),d0
	ext.w	d0
	cmpi.w	#$2B,d0
	blt.s	loc_0114F8
	move.w	#$2B,d0
	move.b	d0,$8(a6)
	tst.w	$26(a6)
	beq.s	loc_0114EE
	addq.w	#$2,d0
loc_0114EE:
	bchg	#$4,$25(a6)
	beq.s	loc_0114F8
	addq.w	#$1,d0
loc_0114F8:
	move.w	$26(a6),d1
	beq.s	loc_01150C
	cmpi.w	#$96,d1
	bge.s	loc_01150C
	bchg	#$5,$25(a6)
	beq.s	loc_011532
loc_01150C:
	cmpi.w	#$1,$28(a6)
	bcs.s	loc_01151E
	adda.w	#$480,a0
	beq.s	loc_01151E
	adda.w	#$480,a0
loc_01151E:
	move.w	d0,d1
	andi.w	#$7,d0
	eor.w	d0,d1
	mulu.w	#$90,d0
	add.w	d1,d0
	lsr.w	#$1,d1
	add.w	d1,d0
	adda.w	d0,a0
loc_011532:
	lea.l	($FFFFEA44).l,a2
	move.w	#$5,d6
loc_01153C:
	move.w	#$1,d7
	movea.l	a0,a3
loc_011542:
	move.w	(a3),d0
	lsl.w	#$5,d0
	lea.l	(a1,d0.w),a4
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	lea.l	$48(a3),a3
	dbra	d7,loc_011542
	lea.l	$20(a2),a2
	addq.w	#$2,a0
	dbra	d6,loc_01153C
	rts
