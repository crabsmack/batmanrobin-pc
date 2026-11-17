; ---------------------------------------------------------------------------
; ToyRing_SpawnRandom - spawner (5 blocks) of a toy ring of random size (gosub $0392CC: 4-7 figures, radius $1E-$3C), energy $C0, drop action $25, which later splits into single figures (coroutine 1 $0395F4)
; In:  d4/d5 = position (wave spawn entry)
; Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_039186, d7 = 5
; Evidence: Op2_Gosub $0392CC (Op2_SwitchRandom over four ToyRing_Init calls); Op2_SetSpawnAction $25; wave script Scr5_034932 (7 entries); TAS f80988-f83339 (5 frames)
; ---------------------------------------------------------------------------
ToyRing_SpawnRandom:
	lea.l	(Scr2_039186).l,a2
	move.w	#$5,d7
	jmp	(Obj_SpawnEnemyN).l
