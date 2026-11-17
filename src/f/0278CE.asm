; ---------------------------------------------------------------------------
; Enemy_LadderThug_SpawnB - spawner: rope-ladder thug Scr4_0278E2 (an a2 script: throw set $0278EA: one of 6 bomb arcs, body $02653E)
; In:  d4/d5 = position
; Out: d0 = 0 (quits the caller when the pool is full)
; Evidence: dc.l at $021BD0 / $021CB2 and op $2CC4 at $0273E6 (boss phase 2); TAS f33599 (one spawn)
; ---------------------------------------------------------------------------
Enemy_LadderThug_SpawnB:
	lea.l	(Scr4_0278E2).l,a2
	move.w	#$6,d7
	jsr	(Obj_SpawnEnemyNOrQuit).l
	jmp	Enemy_LadderThug_InitLadder(pc)
