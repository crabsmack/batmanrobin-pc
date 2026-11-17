; ---------------------------------------------------------------------------
; Enemy_PoleThug_SpawnC - lift trigger callback: pole-sliding thug variant Scr2_02568C that never shoots from the pole: holds $780 frames, slides, repeats until it reaches its jump-off y
; In:  d4/d5 = trigger x/y, a1 = entry words after the callback
; Out: d0 = 0 (quits the caller when the pool is full)
; Evidence: 20 dc.l uses $02159E-$021BFC in the lift trigger table (the most common variant); body $02577E; TAS f21822-f29613 (21 frames)
; ---------------------------------------------------------------------------
Enemy_PoleThug_SpawnC:
	lea.l	(Scr2_02568C).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage2_SetSpawnVarsFromEntry(pc)
