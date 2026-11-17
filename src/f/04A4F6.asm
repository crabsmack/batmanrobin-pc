; ---------------------------------------------------------------------------
; StageCard_SpawnSubtitle4 - spawns the SNOW IN JULY? subtitle object (sprite text sliding in from the right)
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A502 Text_DrawAtObject string $140EF 'SNOW IN JULY?'; entered by the stage 4 director
; ---------------------------------------------------------------------------
StageCard_SpawnSubtitle4:
	lea.l	(Scr2_04A502).l,a2
	jmp	(Obj_SpawnEnemy).l
