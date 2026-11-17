; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnHoverAboveLevel - wave spawner: a red gun drone (Scr2_02C52C, cruise $255) that holds Enemy_RedDrone_SpawnHoverAbove's window (y -$64..-$46, x +$AA..+$C8) for $F00 ticks, then one level with the player (y +$14, x +$D2..+$F0) for $F00, then leaves
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $0225EE; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnHoverAboveLevel:
	lea.l	(Scr2_02C52C).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
