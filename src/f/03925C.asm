; ---------------------------------------------------------------------------
; ToyRing_SpawnRandomAlt - spawner (5 blocks) of a random-size toy ring (gosub $0392CC) like ToyRing_SpawnRandom but with the hop coroutine $039508 and charge cycle $038074, drop action $25
; In:  d4/d5 = position (trigger or wave entry)
; Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_03926C, d7 = 5
; Evidence: Scr2_03926C: Op2_SetSpawnAction $25, Op2_Gosub $0392CC, coroutines 2/3/1 = $039508/$038074/$0395F4; 23 table entries (Tbl_033CBE, Tbl_034142, Scr5_034932); TAS f79584-f112848 (13 frames)
; ---------------------------------------------------------------------------
ToyRing_SpawnRandomAlt:
	lea.l	(Scr2_03926C).l,a2
	move.w	#$5,d7
	jmp	(Obj_SpawnEnemyN).l
