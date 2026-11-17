; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnRiseA - wave spawner: a red gun drone (Scr2_02C1C2) that enters from below the screen, rises 100 px, then holds a window above-right of the player and then below-right (x +$14..+$D2) for $780 ticks each, tracks its height and leaves; fires 4 pairs of shots $168 apart once (then waits $5640)
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: entries y $E0E8 (screen $E8, below the 224-line screen) at x $96/$FA in Tbl_0234DC / Scr5_0235B4; first leg (y, x) = (-$64, 0); attack $2C25C; TAS 47030-47122
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnRiseA:
	lea.l	(Scr2_02C1C2).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
