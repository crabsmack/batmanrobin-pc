; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnHoverBelowAbove - wave spawner: a red gun drone (Scr2_02C634, cruise $255): a window below-right (y +$46..+$64), then above-right (y -$64..-$46) of the player (x +$AA..+$C8), $F00 ticks each, then leaves
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $022602; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnHoverBelowAbove:
	lea.l	(Scr2_02C634).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
