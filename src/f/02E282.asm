; ---------------------------------------------------------------------------
; Enemy_Missile_SpawnFromPodC - spawns a homing missile like Enemy_Missile_SpawnFromPodA with turn rate $600 (Scr2_02E292: cruise $300)
; In:  d4/d5 = x/y (the pod's), g_VM2Arg0-2
; Out: Obj_SpawnEnemyOrQuit's results, then Enemy_Missile_InitFromArgs's
; Evidence: spawner long(s) at $02E278; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Enemy_Missile_InitFromArgs(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_Missile_SpawnFromPodC:
	lea.l	(Scr2_02E292).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Enemy_Missile_InitFromArgs(pc)
