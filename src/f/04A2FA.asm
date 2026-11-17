; ---------------------------------------------------------------------------
; StageCard_SpawnDirector2 - spawns stage 2's card director (Scr2_04A306): scene end after $C8 frames, palette line 3, then the LEVEL TWO title and the A TWO SIDED STORY subtitle 30 frames apart
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A306 = the stage 1 director's layout (Scene_EndAfterDelay $C8, Op2_LoadPalette3, two CallNativeRelSaved +$10/+$3A); called with jsr from StageCard_Stage2
; ---------------------------------------------------------------------------
StageCard_SpawnDirector2:
	lea.l	(Scr2_04A306).l,a2
	jmp	(Obj_SpawnEnemy).l
