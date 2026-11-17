; ---------------------------------------------------------------------------
; Enemy_LadderThug_SpawnA - spawner: rope-ladder thug Scr2_0278C6 (throw set $0278FA: one of 3 bomb arcs, body $02653E)
; In:  d4/d5 = position
; Out: d0 = 0 (quits the caller when the pool is full)
; Evidence: dc.l at $021BC8 / $021CAA in the lift trigger table and op $2CC4 at $0273DC in the boss's phase-2 coroutine; TAS f24981-f33179 (6)
; ---------------------------------------------------------------------------
Enemy_LadderThug_SpawnA:
	lea.l	(Scr2_0278C6).l,a2
	move.w	#$6,d7
	jsr	(Obj_SpawnEnemyNOrQuit).l
	jmp	Enemy_LadderThug_InitLadder(pc)
