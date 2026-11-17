; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnHoverBelow - wave spawner: a red gun drone (Scr2_02C47A) that holds a window below-right of the targeted player (y +$46..+$64, x +$AA..+$C8) for $F00 ticks, then leaves; holds the shared attack $2C4DE
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: window words ($64, $46, $C8, $AA); entries in Scr5_022780 +$14C.. and Scr5_021D3A +$7C4 (drop $1A); not run in the TAS (fuzz runs)
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnHoverBelow:
	lea.l	(Scr2_02C47A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
