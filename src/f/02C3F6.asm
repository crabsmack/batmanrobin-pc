; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnHoverLevel - wave spawner: a red gun drone (Scr2_02C406) that holds a window level with the targeted player (y +$14, x +$DC..+$E6) for $F00 ticks, then leaves like Enemy_RedDrone_SpawnHoverAbove (shared attack $2C4DE)
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: window words ($14, $14, $E6, $DC); entries next to HoverAbove/HoverBelow in Scr5_022780 and Scr5_021D3A +$7BA (drop $17); not run in the TAS (fuzz runs)
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnHoverLevel:
	lea.l	(Scr2_02C406).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
