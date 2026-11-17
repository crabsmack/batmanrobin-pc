; ---------------------------------------------------------------------------
; StageCard_SpawnDirector4 - spawns stage 4's card director (Scr2_04A48C): scene end after $C8 frames, palette line 3, then the LEVEL FOUR title and the SNOW IN JULY? subtitle 30 frames apart
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A48C: Scene_EndAfterDelay $C8, Op2_LoadPalette3, two CallNativeRelSaved +$10/+$3A; called from StageCard_Stage4
; ---------------------------------------------------------------------------
StageCard_SpawnDirector4:
	lea.l	(Scr2_04A48C).l,a2
	jmp	(Obj_SpawnEnemy).l
