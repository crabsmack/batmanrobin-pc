; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverBelow - wave spawner: a black gun drone (Scr2_02A082, body $27DBE, cruise $2AA) that holds one window below-right of the targeted player (y +$28..+$50, x +$F0..+$10E) for $780 frames, then leaves
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $023BCE; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverBelow:
	lea.l	(Scr2_02A082).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
