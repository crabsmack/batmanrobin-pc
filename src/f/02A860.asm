; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverRandomC - wave spawner: HoverRandomA's drone (Scr2_02A870) with its three windows at x +$B4..+$BE
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $0230E2, $02310A, $023128, $023146; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverRandomC:
	lea.l	(Scr2_02A870).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
