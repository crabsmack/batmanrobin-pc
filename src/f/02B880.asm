; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnSweepA - wave spawner: a red gun drone (Scr2_02B890, cruise y $2AA, x $355) on 2 laps of legs (dx, dy) (-$32, 0), ($D2, $C6), (0, -$90), ($32, 0), then (0, $13C)
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $022150-$022182 (6); lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnSweepA:
	lea.l	(Scr2_02B890).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
