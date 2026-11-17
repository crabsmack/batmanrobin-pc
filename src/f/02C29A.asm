; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnRiseB - wave spawner: the second rise-in red drone (Scr2_02C2AA): rises 100 px from below the screen, holds a window below-right then above-right of the player (x +$B4..+$D2) for $780 ticks each, tracks its height and leaves; shares the attack $2C25C
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: entries y $E0E8 at x $C8/$12C in Tbl_0234DC / Scr5_0235B4; TAS 47076-47530
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnRiseB:
	lea.l	(Scr2_02C2AA).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
