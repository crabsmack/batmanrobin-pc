; ---------------------------------------------------------------------------
; Story_AsylumGate - first scene of the idle story list (Tbl_00A136): the Arkham Asylum gate under the moon; plain scene init + art, the camera object, then the skippable scene loop
; In:  none
; Out: via Scene_LoopSkippable (Start skips; the object's Scene_EndAfterDelay $12C ends it)
; Evidence: Tbl_00A136 entry 1; Scr6_047CD0/Scr6_047D2A; idle run 1722 + 3925n, scenario-ending f140119/144053 (png f140400: the ARKHAM ASYLUM arch and the moon)
; ---------------------------------------------------------------------------
Story_AsylumGate:
	lea.l	Scr6_047CD0(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_047D2A(pc),a6
	jsr	(VM6_Run).l
	bsr.w	Story_SpawnGateCamera
	bra.w	Scene_LoopSkippable
