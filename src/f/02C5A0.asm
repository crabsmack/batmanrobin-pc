; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnHoverLevelBelow - wave spawner: a red gun drone (Scr2_02C5B0, cruise $255): a window level with the player (y +$14, x +$D2..+$F0), then below-right (y +$46..+$64, x +$AA..+$C8), $F00 ticks each, then leaves
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $0225F8; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnHoverLevelBelow:
	lea.l	(Scr2_02C5B0).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
