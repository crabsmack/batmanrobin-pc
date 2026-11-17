; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnWaypointsB - wave spawner: the gun drone with WaypointsA's course mirrored vertically (Scr2_029F8A): relative (dx, dy) (-$96, -$64), the absolute point (x $C8, y $60), relative (+$28, -$28), (+$14, +$14), the absolute point (x $64, y $18C), then relative (0, +$258) down off the screen, firing every $168 frames, the upper gun first (vars 0/1 = 0)
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs2 words are (dy, dx); pairs 2 and 5 go to $27DA8 (Op2_AimAtArgPoint, absolute; A's y $12C/0 mirrored about y $C6), the rest to $27D90/$27D98 (Op2_AimAtTargetArgOffset, relative); 12 entries in Scr5_021D3A (+$640.., y $2178), interleaved with WaypointsA; TAS 37005-37943
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnWaypointsB:
	lea.l	(Scr2_029F8A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
