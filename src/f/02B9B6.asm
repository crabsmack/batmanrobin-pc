; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnSweepB - wave spawner: SweepA's red drone (Scr2_02B9C6) at cruise $355 both ways, 3 laps, leaving by (0, $9E)
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $022A26-$022A58 (6); lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnSweepB:
	lea.l	(Scr2_02B9C6).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
