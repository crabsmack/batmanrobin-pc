; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnWaypointsA - wave spawner: a black gun drone (Scr2_029F0C, body $27DBE: energy $68, score $C8) that flies a course of 6 waypoints: relative (dx, dy) (-$96, +$64), the absolute point (x $C8, y $12C), relative (+$28, +$28), (+$14, -$14), the absolute point (x $64, y 0) and finally relative (0, -$258) up off the screen, firing every $168 frames, the lower gun first (vars 0/1 = 1: the shot counter; Scr2_027E58 fires the upper gun on odd counts, the lower on even)
; In:  d4/d5 = x/y; a1 = the wave entry's word (attack delay, timer 1)
; Out: as Obj_SpawnEnemyOrQuit, then Spawn_SetTimer1FromEntry (timer 1 from the entry)
; Evidence: Op2_SetVector2HereGotoArgs2 word pairs (y, x) into Op2_AimAtTargetArgOffset $27D90/$27D98 (Arg0 = dx, Arg1 = dy), except pairs 2 and 5, which go to $27DA8 (Op2_AimAtArgPoint: Arg0/Arg1 = absolute x/y); 12 entries in Scr5_021D3A (+$5C2.., y $2014, drop action 1 or $2C); frames $39BC/$39C6 rendered: a black pod with a cannon pointing left; TAS 36669-38223
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnWaypointsA:
	lea.l	(Scr2_029F0C).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
