; ---------------------------------------------------------------------------
; Enemy_PoleThug_SpawnE - lift trigger callback: spawns a pole-sliding thug variant (Scr2_02571C: energy $30/$C0, timers $1E0, $140, $280 x2 loops, $258, $50) on the shared body, then script vars 0/1 from the entry
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $0217C8, $021804, $021ABA, $021AF6; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Stage2_SetSpawnVarsFromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_PoleThug_SpawnE:
	lea.l	(Scr2_02571C).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage2_SetSpawnVarsFromEntry(pc)
