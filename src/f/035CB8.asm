; ---------------------------------------------------------------------------
; Enemy_BombRabbit_SpawnDropIn - wave spawner of a bomb rabbit that drops in from above (script Scr2_035CC4: coroutine 0 = fall op $1CDC, then the walk anim; cruise x $1AA; the small touch box starts its fuse)
; In:  d4/d5 = spawn position (wave entry)
; Out: Obj_SpawnEnemy's result
; Evidence: wave spawn entries $034E54-$034E74; shared body $035DA2 (energy $80, score $64); TAS f82614-f115324 (3 spawns); png f082680 shows the grey rabbits, the bomb ball drawn on them (frames $4718/$4722)
; ---------------------------------------------------------------------------
Enemy_BombRabbit_SpawnDropIn:
	lea.l	(Scr2_035CC4).l,a2
	jmp	(Obj_SpawnEnemy).l
