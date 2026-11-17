; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverRandomB - wave spawner: HoverRandomA's drone (Scr2_02A7CC) with its three windows nearer the player (x +$8C..+$96)
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $0230EC, $023114, $023132, $023150; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverRandomB:
	lea.l	(Scr2_02A7CC).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
