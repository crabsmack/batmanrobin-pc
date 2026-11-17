; ---------------------------------------------------------------------------
; Story_SpawnGateCamera - spawns the gate scene's camera object (Scr2_04853A): ends the scene after $12C, rises $120 px in a $D20-tick arc (Op2_JumpAnim) and draws two RAM-frame sprites at its drawn position; Story_GateScroll turns its y into the plane scroll
; In:  none
; Out: jumps to Obj_SpawnEnemy
; Evidence: Scr2_04853A: CallNative Scene_EndAfterDelay $12C, SetAnim, JumpAnim $FEE0 $D20, CallNativeRel Story_GateScroll every frame; callers Story_AsylumGate and Ending_AsylumGate; idle 1722 + 3925n
; ---------------------------------------------------------------------------
Story_SpawnGateCamera:
	lea.l	(Scr2_04853A).l,a2
	jmp	(Obj_SpawnEnemy).l
