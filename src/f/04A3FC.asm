; ---------------------------------------------------------------------------
; StageCard_SpawnTitle3 - spawns the LEVEL THREE title object (sprite text sliding in from the left)
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A408 Text_DrawAtObject string $140CE 'LEVEL THREE'; entered by the stage 3 director
; ---------------------------------------------------------------------------
StageCard_SpawnTitle3:
	lea.l	(Scr2_04A408).l,a2
	jmp	(Obj_SpawnEnemy).l
