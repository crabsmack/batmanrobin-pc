; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnDropB - wave spawner: the second drop-in red drone (Scr2_02C0DA): drops 100 px from above the screen, then holds a window below-right of the player (y +$32..+$64, x +$B4..+$D2) and then above-right (y -$64..-$32) for $780 ticks each, tracks the player's height and leaves; shares the attack $2C174
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: entries y $DFF8 at x $C8/$12C in Tbl_0234DC / Scr5_0235B4 (next to SpawnDropA); TAS 46591-46638
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnDropB:
	lea.l	(Scr2_02C0DA).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
