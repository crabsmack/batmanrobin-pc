; ---------------------------------------------------------------------------
; Story_TowerScroll - native of the tower scene object: plane B V-scroll = -y (plane B follows the object's drop)
; In:  a6 = object
; Out: g_VScrollB
; Evidence: neg.w; only caller Op2_CallNativeRel at $048702 in Scr2_0486DE; scenario-ending 141608-145679 (280 frames)
; ---------------------------------------------------------------------------
Story_TowerScroll:
	move.w	$42(a6),d0
	neg.w	d0
	move.w	d0,(g_VScrollB).w
	rts
; ---------------------------------------------------------------------------
; Story_AsylumFacade - fifth story scene: the asylum's wall with barred windows and a figure on it; plain scene init + art, camera object, skippable loop
; In:  none
; Out: via Scene_LoopSkippable
; Evidence: Tbl_00A136 entry 5 ($0487B2, inside the old $0487A6 file); Scr6_047D06/047DF4; idle png f11400 in the window 11185-11525
; ---------------------------------------------------------------------------
Story_AsylumFacade:
	lea.l	Scr6_047D06(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_047DF4(pc),a6
	jsr	(VM6_Run).l
	bsr.w	Story_SpawnFacadeCamera
	bra.w	Scene_LoopSkippable
