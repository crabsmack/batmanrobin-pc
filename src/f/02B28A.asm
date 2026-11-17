; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnCircuitC - wave spawner: CircuitC's course flown by a red gun drone (Scr2_02B29A, body $27F70)
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $0234AE, $0234C2; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnCircuitC:
	lea.l	(Scr2_02B29A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
