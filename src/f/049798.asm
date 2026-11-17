; ---------------------------------------------------------------------------
; Title_StartPaletteFade - creates the title's palette fade-in object (handler Title_PaletteFadeRun): a6+$A = the palette data in the decompressed buffer at $FF0000 (its first entry), then 7 channels from Tbl_04984C (delay, start level -$10, record pointer) at a6+$E, 10 bytes each
; In:  -
; Out: a6 = the new object (Obj_NewClear)
; Evidence: only caller Title_Run; channel records (period, reload flag $F4CA/$F4CE/$F4D2, colours - 1, buffer offset, g_PalSource destination) read by Title_PaletteFadeChannel; every title pass (TAS f404, idle f1172...)
; ---------------------------------------------------------------------------
Title_StartPaletteFade:
	lea.l	(Title_PaletteFadeRun).l,a0
	jsr	(Obj_NewClear).l
	lea.l	($FF0008).l,a0
	adda.l	(a0),a0
	addq.l	#$2,a0
	move.l	a0,$A(a6)
	lea.l	(Tbl_04984C).l,a0
	lea.l	$E(a6),a5
	moveq	#$6,d7
loc_0497BE:
	move.w	(a0)+,(a5)
	move.w	(a0)+,$4(a5)
	clr.w	$2(a5)
	move.l	a0,$6(a5)
	lea.l	$A(a0),a0
	lea.l	$A(a5),a5
	dbra	d7,loc_0497BE
	rts
