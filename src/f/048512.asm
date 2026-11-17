; ---------------------------------------------------------------------------
; Ending_AsylumGate - first ending scene (Main_Ending, after music $12): the same Arkham gate picture and camera object as Story_AsylumGate, but in the unskippable loop
; In:  none
; Out: via Scene_LoopUnskippable (the object's Scene_EndAfterDelay ends it)
; Evidence: jsr from Main_Ending before $4A626 (IN A FREEZING CELL IN ARKHAM ASYLUM... text, f132000); same Scr6_047CD0/047D2A and Story_SpawnGateCamera; scenario-ending f131386, scenario-level-skip f14740
; ---------------------------------------------------------------------------
Ending_AsylumGate:
	lea.l	Scr6_047CD0(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_047D2A(pc),a6
	jsr	(VM6_Run).l
	bsr.w	Story_SpawnGateCamera
	bra.w	Scene_LoopUnskippable
