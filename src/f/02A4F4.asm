; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnWaypointsG - wave spawner: a black gun drone (Scr2_02A504) on WaypointsE's course leaving at (+$A0, +$258) (not +$1F4); 2 rounds
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $022268, $022BEE, $022BF8; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnWaypointsG:
	lea.l	(Scr2_02A504).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
