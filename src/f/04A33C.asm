; ---------------------------------------------------------------------------
; StageCard_SpawnTitle2 - spawns the LEVEL TWO title object (sprite text sliding in from the left)
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A348 Text_DrawAtObject string $140B2 'LEVEL TWO'; entered by the stage 2 director's CallNativeRelSaved; corpus runs reaching stage 2 (scenario-level-skip)
; ---------------------------------------------------------------------------
StageCard_SpawnTitle2:
	lea.l	(Scr2_04A348).l,a2
	jmp	(Obj_SpawnEnemy).l
