; ---------------------------------------------------------------------------
; Enemy_PoleThug_SpawnA - lift trigger callback: spawns the pole-sliding thug variant Scr2_0255C2 (hold, slide and shoot from the pole in a 3x3 counter loop) on the shared body $02577E, then script vars 0/1 from the entry
; In:  d4/d5 = trigger x/y, a1 = entry words after the callback
; Out: d0 = 0 (quits the caller when the pool is full)
; Evidence: dc.l at $0214B8, $0214C4, $02156E, $02157A, $021810, $02181C in the lift's y-trigger table (x $28 / $118 = the left / right side pipe, y -$12, var0 $7FFF, var1 $AA/$64 = jump-off y); TAS f21360-f22492; png f21310 shows a thug on the left pipe at the top
; ---------------------------------------------------------------------------
Enemy_PoleThug_SpawnA:
	lea.l	(Scr2_0255C2).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage2_SetSpawnVarsFromEntry(pc)
