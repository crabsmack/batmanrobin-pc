; ---------------------------------------------------------------------------
; Story_SearchlightScroll - native of the searchlight scene object: all 224 plane B lines at x * 3/16; plane A lines at x / 4 + a drift ($46(a6) += $6000 per frame) + a sine wobble (/8, phase $2A(a6) += 2 per frame plus the incoming d2, +$A per line)
; In:  a6 = object, d2 = phase base from the caller
; Out: g_HScrollLinesA/B
; Evidence: Mem_FillWords 224 into g_HScrollLinesB; Tbl_059B22 sine loop over 224 lines; only caller Op2_CallNativeRel at $048610 in Scr2_0485E4; scenario-ending 141217-145528
; ---------------------------------------------------------------------------
Story_SearchlightScroll:
	move.w	$3E(a6),d0
	move.w	d0,d1
	muls.w	#$4000,d1
	swap	d1
	muls.w	#$3000,d0
	swap	d0
	lea.l	(g_HScrollLinesB).l,a0
	move.w	#$E0,d7
	jsr	(Mem_FillWords).l
	lea.l	(Tbl_059B22).l,a0
	lea.l	(g_HScrollLinesA).l,a1
	addq.w	#$2,$2A(a6)
	add.w	$2A(a6),d2
	addi.l	#$6000,$46(a6)
	add.w	$46(a6),d1
	move.w	#$DF,d7
loc_04869C:
	andi.w	#$3FE,d2
	move.b	(a0,d2.w),d0
	ext.w	d0
	asr.w	#$3,d0
	add.w	d1,d0
	move.w	d0,(a1)+
	addi.w	#$A,d2
	dbra	d7,loc_04869C
	rts
; ---------------------------------------------------------------------------
; Story_GuardTower - fourth story scene: the asylum guard tower with its searchlight on the wall; plain scene init + art, camera object, skippable loop
; In:  none
; Out: via Scene_LoopSkippable
; Evidence: Tbl_00A136 entry 4 ($0486B6, inside the old $048656 file); Scr6_047CF4/047DC2; idle png f7200 (tower and beam) in the window 7090-7260
; ---------------------------------------------------------------------------
Story_GuardTower:
	lea.l	Scr6_047CF4(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_047DC2(pc),a6
	jsr	(VM6_Run).l
	bsr.w	Story_SpawnTowerCamera
	bra.w	Scene_LoopSkippable
