; ---------------------------------------------------------------------------
; StageCard_SpawnDirector1 - spawns stage 1's card director (Scr2_04A23E): ends the scene after $C8 frames, loads palette line 3, then spawns the title after 30 frames and the subtitle 30 frames later
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A23E: CallNative Scene_EndAfterDelay $C8, Op2_LoadPalette3, coroutine SetTimer0 $1E0 (30 frames at speed $100) / CallNativeRelSaved -> StageCard_SpawnTitle1 / StageCard_SpawnSubtitle1; f773 (title f802, subtitle f832)
; ---------------------------------------------------------------------------
StageCard_SpawnDirector1:
	lea.l	(Scr2_04A23E).l,a2
	jmp	(Obj_SpawnEnemy).l
