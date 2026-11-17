; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnTrackYB - wave spawner: Enemy_RedDrone_SpawnTrackY's red gun drone (Scr2_02A914, body $27F70) flying in at cruise x $255 (not $200), then following the player's height for $F00 frames
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $0230F6; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnTrackYB:
	lea.l	(Scr2_02A914).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
