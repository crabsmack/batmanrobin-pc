; ---------------------------------------------------------------------------
; Continue_SpawnController - spawns the continue screen's controller object (Scr2_0491A0): hovers on an oscillator and branches on the credits (Continue_BranchHasCredits) into the countdown/Start script
; In:  none
; Out: jumps to Obj_SpawnEnemy
; Evidence: Scr2_0491A0: SetPos 0 0, AllocOsc/SetOscY, then Op2_Native into Continue_BranchHasCredits; scenario-continue f41999
; ---------------------------------------------------------------------------
Continue_SpawnController:
	lea.l	(Scr2_0491A0).l,a2
	jmp	(Obj_SpawnEnemy).l
