; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnCircuitC - wave spawner: a black gun drone (Scr2_02B0AE, cruise $3AA) on a circuit of legs (Op2_SetVector2HereGotoArgs2) starting (dx, dy) ($BE, $118), ($1E, $14), ($1E, -$14), ($50, $92) ...
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at 10 wave entries $0227F4-$0234B8; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnCircuitC:
	lea.l	(Scr2_02B0AE).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
