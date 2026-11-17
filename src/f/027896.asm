; ---------------------------------------------------------------------------
; Enemy_LadderThug_SpawnE - boss-phase-1 spawner: a rope-ladder thug (Scr2_0278AA: throw set $0278EA, one of 6 bomb arcs, body $02653E: Enemy_LadderThug_SpawnB's) hung from the boss hull
; In:  d4/d5 = x/y (the boss's anchor)
; Out: Obj_SpawnEnemyNOrQuit's results, then Enemy_LadderThug_InitLadder's
; Evidence: spawner long(s) at Op2_CallNativeRelSaved at $026E9C, $026F0E, $026F4A ... (7); lea script / move.w #6,d7 / jsr Obj_SpawnEnemyNOrQuit / jmp Enemy_LadderThug_InitLadder(pc) as Enemy_LadderThug_SpawnC; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_LadderThug_SpawnE:
	lea.l	(Scr2_0278AA).l,a2
	move.w	#$6,d7
	jsr	(Obj_SpawnEnemyNOrQuit).l
	jmp	Enemy_LadderThug_InitLadder(pc)
