; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverAbove - wave spawner: a black gun drone (Scr2_02A0EC, body $27DBE, cruise $2AA) that holds one window above-right of the targeted player (y -$50..-$28, x +$F0..+$10E) for $780 frames, then leaves
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $023BD8; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverAbove:
	lea.l	(Scr2_02A0EC).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
