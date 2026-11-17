; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnStraightFast - spawns a fast, nearly straight homing missile (Scr2_02D002: cruise $5AA, turn rate $100, the missile body) with heading, straight and homing times from g_VM2Arg0-2
; In:  d4/d5 = x/y (the launcher's), g_VM2Arg0-2
; Out: Obj_SpawnEnemyOrQuit's results, then Enemy_Missile_InitFromArgs's
; Evidence: spawner long(s) at $02CF4C; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Enemy_Missile_InitFromArgs(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnStraightFast:
	lea.l	(Scr2_02D002).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
