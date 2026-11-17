; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnWaypointsE - wave spawner: a black gun drone (Scr2_02A40A) on Enemy_BlackDrone_SpawnWaypointsC's relative waypoints (dx, dy) (-$28, 0) at cruise $255, (-$28, -$32) at $355, (-5, +6) at $3AA, then (+$1F4, +$258) off the screen; 3 rounds
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $022B8E; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnWaypointsE:
	lea.l	(Scr2_02A40A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
