; ---------------------------------------------------------------------------
; StageCard_SpawnSubtitle2 - spawns the A TWO SIDED STORY subtitle object (sprite text sliding in from the right)
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A37C Text_DrawAtObject string $140BC 'A TWO SIDED STORY'; entered by the stage 2 director
; ---------------------------------------------------------------------------
StageCard_SpawnSubtitle2:
	lea.l	(Scr2_04A37C).l,a2
	jmp	(Obj_SpawnEnemy).l
