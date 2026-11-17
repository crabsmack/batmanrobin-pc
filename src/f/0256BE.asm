; ---------------------------------------------------------------------------
; Enemy_PoleThug_SpawnD - lift trigger callback: pole-sliding thug variant Scr2_0256CE: holds $1E0, slides, then jumps off the pole at once ($02592E) wherever it is
; In:  d4/d5 = trigger x/y, a1 = entry words after the callback
; Out: d0 = 0 (quits the caller when the pool is full)
; Evidence: 18 dc.l uses $0214D0-$021D1E; body $02577E; TAS f21270-f29037; RAM f21300: an object at x 32, y 18 with var1 $87
; ---------------------------------------------------------------------------
Enemy_PoleThug_SpawnD:
	lea.l	(Scr2_0256CE).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage2_SetSpawnVarsFromEntry(pc)
