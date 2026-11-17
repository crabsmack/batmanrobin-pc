; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnDropA - wave spawner: a red gun drone (Scr2_02C030) that enters from above the screen, drops 100 px, then holds a window above-right of the player (y -$64..-$32, x +$14..+$D2) and one below-right (y +$32..+$64) for $780 ticks each, then follows the player's height $F00 ticks and leaves; fires 4 pairs of shots $168 apart, then pauses $B40
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: entries y $DFF8 (screen -8) at x $96/$FA in Tbl_0234DC and Scr5_0235B4, alternating with Enemy_RedDrone_SpawnDropB; Op2_SetVector2HereGotoArgs2 ($64, 0) = (y, x) via g_VM2Arg1/Arg0; windows through $27D34; attack $2C174; TAS 46683-47530
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnDropA:
	lea.l	(Scr2_02C030).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
