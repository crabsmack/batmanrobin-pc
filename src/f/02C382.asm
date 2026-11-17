; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnHoverAbove - wave spawner: a red gun drone (Scr2_02C392, cruise $255) that holds a window above-right of the targeted player (y -$64..-$46, x +$AA..+$C8) for $F00 ticks, then moves 40 px left, tracks the player's height $F00 ticks and leaves; fires 4 pairs of shots $F0 apart, then pauses $780 (attack $2C4DE, shared)
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: window words (y far, y near, x far, x near) = (-$46, -$64, $C8, $AA) into $27D34; spawned with ...HoverLevel/...HoverBelow in Scr5_022780 +$138.. and Scr5_021D3A +$7B0 (drop $2E); TAS 37341-37701
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnHoverAbove:
	lea.l	(Scr2_02C392).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
