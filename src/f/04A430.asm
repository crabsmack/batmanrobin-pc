; ---------------------------------------------------------------------------
; StageCard_SpawnSubtitle3 - spawns the TEA TIME! subtitle object (sprite text sliding in from the right)
; In:  none
; Out: via Obj_SpawnEnemy
; Evidence: Scr2_04A43C Text_DrawAtObject string $140DA 'TEA TIME!'; entered by the stage 3 director
; ---------------------------------------------------------------------------
StageCard_SpawnSubtitle3:
	lea.l	(Scr2_04A43C).l,a2
	jmp	(Obj_SpawnEnemy).l
