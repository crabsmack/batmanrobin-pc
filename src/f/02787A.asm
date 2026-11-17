; ---------------------------------------------------------------------------
; Enemy_LadderThug_SpawnD - boss-phase-1 spawner: a rope-ladder thug (Scr2_02788E: throw set $027C40, body $026566) hung from the boss hull
; In:  d4/d5 = x/y (the boss's anchor)
; Out: Obj_SpawnEnemyNOrQuit's results, then Enemy_LadderThug_InitLadder's
; Evidence: spawner long(s) at Op2_CallNativeRelSaved at $026E72, $026EE4, $026F3C ... (7); lea script / move.w #6,d7 / jsr Obj_SpawnEnemyNOrQuit / jmp Enemy_LadderThug_InitLadder(pc) as Enemy_LadderThug_SpawnC; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_LadderThug_SpawnD:
	lea.l	(Scr2_02788E).l,a2
	move.w	#$6,d7
	jsr	(Obj_SpawnEnemyNOrQuit).l
	jmp	Enemy_LadderThug_InitLadder(pc)
