; ---------------------------------------------------------------------------
; ToyRing_SpawnBig - spawner (5 blocks, Obj_SpawnEnemyN) of the big toy ring: 16 figures, radius $6E, energy $400, 1000 points (Scr2_03905E), shared ring body Scr2_037588
; In:  d4/d5 = position (trigger entry)
; Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_03905E, d7 = 5
; Evidence: ToyRing_Init words $10 $6E $40 3 $CCCC; Op2_SetEnergy $400, Op2_SetScoreValue $3E8; Tbl_034142 (+$3C0, +$42E); TAS f109594 (1 frame)
; ---------------------------------------------------------------------------
ToyRing_SpawnBig:
	lea.l	(Scr2_03905E).l,a2
	move.w	#$5,d7
	jmp	(Obj_SpawnEnemyN).l
