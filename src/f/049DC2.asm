; ---------------------------------------------------------------------------
; SegaLogo_SpawnCycler - spawns the SEGA logo object Scr2_049DCE: ends the scene after 120 frames and rotates the logo's colours (SegaLogo_RotatePalette) every 3 frames
; In:  -
; Out: a6 = the object (Obj_SpawnEnemy)
; Evidence: Scr2_049DCE: Scene_EndAfterDelay 0/$78, Op2_SetSpeed $100, op $11BC $A (timer 3 tick, skip while running), Op2_SetTimer3 $30, Op2_CallNativeRel 6 -> $49DEC; TAS f18
; ---------------------------------------------------------------------------
SegaLogo_SpawnCycler:
	lea.l	(Scr2_049DCE).l,a2
	jmp	(Obj_SpawnEnemy).l
