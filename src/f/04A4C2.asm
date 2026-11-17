; ---------------------------------------------------------------------------
; StageCard_SpawnTitle4 - spawns the LEVEL FOUR title object (sprite text sliding in from the left)
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A4CE Text_DrawAtObject string $140E4 'LEVEL FOUR'; entered by the stage 4 director
; ---------------------------------------------------------------------------
StageCard_SpawnTitle4:
	lea.l	(Scr2_04A4CE).l,a2
	jmp	(Obj_SpawnEnemy).l
