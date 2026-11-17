; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnWaypointsF - wave spawner: WaypointsE mirrored vertically (Scr2_02A49A): (-$28, 0) at cruise $2AA, (-$28, +$32) at $355, (-5, -6) at $3AA, then (+$1F4, -$258) off the screen
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $022B98; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnWaypointsF:
	lea.l	(Scr2_02A49A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
