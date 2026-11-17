; ---------------------------------------------------------------------------
; Story_FacadeScroll - native of the facade camera: plane A V-scroll = y - $68 + shake, plane B = y / 4 + shake / 2
; In:  a6 = camera object
; Out: g_VScrollA/B
; Evidence: muls #$4000 + swap; only caller Op2_CallNativeRel at $0487F6 in Scr2_0487DA; scenario-ending 141778-145772
; ---------------------------------------------------------------------------
Story_FacadeScroll:
	move.w	$42(a6),d0
	move.w	d0,d1
	subi.w	#$68,d0
	muls.w	#$4000,d1
	swap	d1
	move.w	(g_ShakeY).w,d2
	add.w	d2,d0
	asr.w	#$1,d2
	add.w	d2,d1
	move.w	d0,(g_VScrollA).w
	move.w	d1,(g_VScrollB).w
	rts
; ---------------------------------------------------------------------------
; Story_Monitors - second story scene: the asylum's four security monitors (static, then the ALARM panel); plain scene init + art, camera object, skippable loop
; In:  none
; Out: via Scene_LoopSkippable
; Evidence: Tbl_00A136 entry 2 ($04885C, inside the old $048838 file); Scr6_047D18/047E32; idle png f2400, scenario-ending png f141000 (ALARM)
; ---------------------------------------------------------------------------
Story_Monitors:
	lea.l	Scr6_047D18(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_047E32(pc),a6
	jsr	(VM6_Run).l
	bsr.w	Story_SpawnMonitorsCamera
	bra.w	Scene_LoopSkippable
