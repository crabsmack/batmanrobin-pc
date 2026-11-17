; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnZigzag - wave spawner: a red gun drone (Scr2_02BE96, cruise $2AA) on legs (dx, dy) ($B0, $5A), ($28, -$14), ($28, $14), (0, $76), ($D8, $C4) ... twice, the last pair picked at random
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $0222A6, $0222B0; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnZigzag:
	lea.l	(Scr2_02BE96).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
