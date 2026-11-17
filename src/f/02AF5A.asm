; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnCircuitB - wave spawner: a black gun drone (Scr2_02AF6A, body $27DBE) on the second circuit (as Enemy_RedDrone_SpawnCircuitB: 30 px left, 2 laps through the absolute points (x $3C, y $50), (x $78, y $13C), (x $50, y $50), (x $3C, y $13C), 4 random up-left exits, 40 px left, track the player's height $F00); its attack $2B078 fires 6 pairs of shot requests $F0 apart, then pauses $2D0
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Scr5_023156 +$1FE/+$208 (x $B4, y $203C, delays $F0/$870); same course words as Scr2_02ACF0 with cruise $3AA; TAS 49847-49998
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnCircuitB:
	lea.l	(Scr2_02AF6A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
