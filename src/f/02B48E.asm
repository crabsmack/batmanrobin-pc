; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnCircuitD - wave spawner: a black gun drone (Scr2_02B49E, cruise $3AA) on CircuitC's legs in the other order (starting ($BE, $74), ($1E, -$14), ($1E, $14), ($50, $FA) ...)
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at 10 wave entries $022808-$0234D6; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnCircuitD:
	lea.l	(Scr2_02B49E).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
