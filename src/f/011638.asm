; ---------------------------------------------------------------------------
; HUD_DrawLifeRing - draws the 3x3-tile ring around the lives digit: segments from health +$4/16 (0-16), two animation tables alternated every 4 frames by counter +$24, then overlays the lives digit (+$6, max 9) on the centre tile
; In:  a6 = player record
; Out: 9 tiles at g_HUDTileBuf, digit at $EAA4; +$24 incremented
; Evidence: called by HUD_StepP1Ring/HUD_StepP2Ring (DMA to VRAM $0520/$0640); TAS frames 3000-90000: ring segments shrink with health ($F0 -> $20) and the digit shows +$6 (2, 3, 4 lives)
; ---------------------------------------------------------------------------
HUD_DrawLifeRing:
	lea.l	(Tbl_058768).l,a0
	move.b	$24(a6),d0
	addq.b	#$1,d0
	btst	#$2,d0
	bne.s	loc_011650
	lea.l	(Tbl_0588CA).l,a0
loc_011650:
	move.b	d0,$24(a6)
	move.w	$4(a6),d0
	bpl.s	loc_01165C
	moveq	#$0,d0
loc_01165C:
	lsr.w	#$4,d0
	cmpi.w	#$10,d0
	ble.s	loc_011666
	moveq	#$10,d0
loc_011666:
	move.w	d0,d1
	add.w	d0,d0
	add.w	d1,d0
	add.w	d0,d0
	adda.w	d0,a0
	lea.l	(Art_058AA2).l,a1
	lea.l	(g_HUDTileBuf).l,a2
	move.w	#$2,d6
loc_011680:
	move.w	#$2,d7
	movea.l	a0,a3
loc_011686:
	move.w	(a3),d1
	lsl.w	#$5,d1
	lea.l	(a1,d1.w),a4
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	move.l	(a4)+,(a2)+
	lea.l	$76(a3),a3
	dbra	d7,loc_011686
	addq.w	#$2,a0
	dbra	d6,loc_011680
	lea.l	(Tbl_058A40).l,a0
	lea.l	(Art_058AA2).l,a1
	lea.l	($FFFFEAA4).l,a2
	move.w	$6(a6),d0
	cmpi.w	#$9,d0
	bcs.s	loc_0116CC
	move.w	#$9,d0
loc_0116CC:
	add.w	d0,d0
	move.w	(a0,d0.w),d0
	lsl.w	#$5,d0
	adda.w	d0,a1
	move.w	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	addq.l	#$1,a1
	addq.l	#$1,a2
	move.w	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	addq.l	#$1,a1
	addq.l	#$1,a2
	move.w	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	addq.l	#$1,a1
	addq.l	#$1,a2
	move.w	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	addq.l	#$1,a1
	addq.l	#$1,a2
	move.w	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	addq.l	#$1,a1
	addq.l	#$1,a2
	move.w	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	addq.l	#$1,a1
	addq.l	#$1,a2
	move.w	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	addq.l	#$1,a1
	addq.l	#$1,a2
	rts
