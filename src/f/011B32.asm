; ---------------------------------------------------------------------------
; BossMeter_SpawnObject - spawns the boss energy meter as a script object running Scr2_011B3E (owner class enemy)
; In:  none
; Out: as Obj_SpawnEnemy (a6 = new object)
; Evidence: only caller BossMeter_Spawn; Scr2_011B3E calls BossMeter_Init, HUD_LockJoin, then loops BossMeter_AnimRing / DrawPercent / QueueTileDMA / DrawSprite through op $2CB2
; ---------------------------------------------------------------------------
BossMeter_SpawnObject:
	lea.l	(Scr2_011B3E).l,a2
	jmp	(Obj_SpawnEnemy).l
