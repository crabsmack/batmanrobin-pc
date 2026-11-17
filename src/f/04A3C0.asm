; ---------------------------------------------------------------------------
; StageCard_SpawnDirector3 - spawns stage 3's card director (Scr2_04A3CC): scene end after $C8 frames, then the LEVEL THREE title and the TEA TIME! subtitle 30 frames apart (no palette-3 load, unlike stages 1, 2, 4)
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A3CC: Scene_EndAfterDelay $C8, two CallNativeRelSaved +$10/+$3A, no $0B76 op; called from StageCard_Stage3
; ---------------------------------------------------------------------------
StageCard_SpawnDirector3:
	lea.l	(Scr2_04A3CC).l,a2
	jmp	(Obj_SpawnEnemy).l
