; ---------------------------------------------------------------------------
; Story_GateScroll - native of the gate camera object: plane A V-scroll = y - $120, plane B = y * 0.14 (both + shake), drawn position = (x $108, y * 0.6 + $140) for its sprites: the vertical pan with parallax
; In:  a6 = camera object
; Out: g_VScrollA/B, $4E/$50(a6)
; Evidence: muls #$2400 / #$9A00 + swap; only caller Op2_CallNativeRel at $048562 in Scr2_04853A; scenario-ending 131416-144402 (960 frames)
; ---------------------------------------------------------------------------
Story_GateScroll:
	move.w	$42(a6),d0
	move.w	d0,d1
	move.w	d0,d2
	subi.w	#$120,d0
	muls.w	#$2400,d1
	swap	d1
	add.w	(g_ShakeY).w,d0
	add.w	(g_ShakeY).w,d1
	move.w	d0,(g_VScrollA).w
	move.w	d1,(g_VScrollB).w
	muls.w	#$9A00,d2
	swap	d2
	addi.w	#$140,d2
	move.w	d2,$50(a6)
	move.w	#$108,$4E(a6)
	rts
; ---------------------------------------------------------------------------
; Story_Searchlight - third story scene: a searchlight lamp in front of the moon; plain scene init + art, camera object, line-scroll mode $C, then the skippable loop that keeps that mode
; In:  none
; Out: via Scene_LoopSkippableKeepMode
; Evidence: Tbl_00A136 entry 3 ($0485B6, a scene-list entry inside the old $04857E file); Scr6_047CE2/047D76; idle png f3000 (moon with a lamp on a pole) in the window 2774-3165
; ---------------------------------------------------------------------------
Story_Searchlight:
	lea.l	Scr6_047CE2(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_047D76(pc),a6
	jsr	(VM6_Run).l
	bsr.w	Story_SpawnSearchlightCamera
	move.w	#$C,(g_ScrollMode).w
	bra.w	Scene_LoopSkippableKeepMode
