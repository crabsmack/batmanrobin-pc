; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnWaypointsH - wave spawner: WaypointsG mirrored vertically (Scr2_02A594): (-$28, 0) at cruise $2AA, (-$28, +$32), (-5, -6), then (+$A0, -$258) off the screen
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $022272, $022C02, $022C0C; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnWaypointsH:
	lea.l	(Scr2_02A594).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
