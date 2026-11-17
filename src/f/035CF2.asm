; ---------------------------------------------------------------------------
; Enemy_BombRabbit_SpawnHop - spawner of a bomb rabbit that hops in (script Scr2_035CFE: jump op $46/$140 then walk at cruise $1AA, the small touch box starts its fuse)
; In:  d4/d5 = spawn position
; Out: Obj_SpawnEnemy's result
; Evidence: wave entries ($034E7C, $035062-$0350CC ...) and the rabbit pot's Spawn_AtOwnOfs at (0, -20) ($03674C, $036782, ...); TAS f82766-f114152 (13 spawns); png f111600: a rabbit climbing out of the pot
; ---------------------------------------------------------------------------
Enemy_BombRabbit_SpawnHop:
	lea.l	(Scr2_035CFE).l,a2
	jmp	(Obj_SpawnEnemy).l
