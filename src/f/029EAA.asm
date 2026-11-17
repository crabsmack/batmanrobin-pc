; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnRandomTurn - spawns a homing missile (Scr2_029EBA: random speed $19-$100, cruise $255, a turn rate of $600/$800/$A00/$C00 at random) with heading, straight and homing times from g_VM2Arg0-2
; In:  d4/d5 = x/y (the launcher's), g_VM2Arg0-2
; Out: Obj_SpawnEnemyOrQuit's results, then Enemy_Missile_InitFromArgs's
; Evidence: spawner long(s) at $029EA0; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Enemy_Missile_InitFromArgs(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnRandomTurn:
	lea.l	(Scr2_029EBA).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
