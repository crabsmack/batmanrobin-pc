; ---------------------------------------------------------------------------
; Enemy_PoleThug_SpawnB - lift trigger callback: pole-sliding thug variant Scr2_025630: random first hold ($78 + [0,$1E0)), then slide, shoot ($0258E2) and hold in $1E0-frame steps
; In:  d4/d5 = trigger x/y, a1 = entry words after the callback
; Out: d0 = 0 (quits the caller when the pool is full)
; Evidence: dc.l at $021586, $021592, $0217A4, $0217E0, $021A96, $021AD2; body $02577E as Enemy_PoleThug_SpawnA; ends in Stage2_SetSpawnVarsFromEntry; TAS f22020-f22092
; ---------------------------------------------------------------------------
Enemy_PoleThug_SpawnB:
	lea.l	(Scr2_025630).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage2_SetSpawnVarsFromEntry(pc)
