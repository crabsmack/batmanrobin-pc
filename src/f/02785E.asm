; ---------------------------------------------------------------------------
; Enemy_LadderThug_SpawnC - boss-phase-1 spawner: rope-ladder thug Scr2_027872 (throw set Scr4_027C28, body $026566) hung from the boss hull
; In:  d4/d5 = position
; Out: d0 = 0 (quits the caller when the pool is full)
; Evidence: op $2CC4 at $026DC0 and $026EDA in the boss's phase-1 attack scripts; tail Enemy_LadderThug_InitLadder; not in the TAS, run at f34230 in the continue/game-over scenarios
; ---------------------------------------------------------------------------
Enemy_LadderThug_SpawnC:
	lea.l	(Scr2_027872).l,a2
	move.w	#$6,d7
	jsr	(Obj_SpawnEnemyNOrQuit).l
	jmp	Enemy_LadderThug_InitLadder(pc)
