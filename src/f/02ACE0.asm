; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnCircuitB - wave spawner: a red gun drone (Scr2_02ACF0, body $27F70) on the second circuit course: Enemy_RedDrone_SpawnCircuit mirrored vertically: 30 px left, then 2 laps through the absolute points (x $3C, y $50), (x $78, y $13C), (x $50, y $50), (x $3C, y $13C) (the circuit's y $13C/$50 swapped, mirrored about y $C6) with the relative legs' dy negated, then one of 4 random relative offsets up-left ((dx, dy) (-$50, -$3C), (-$28, -$28), (-$50, -$50), (-$28, -$78)), 40 px left, tracks the player's height $F00 ticks and leaves; fires two shots left every $F0 ticks, 5 times, then pauses $870
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs2 words are (dy, dx), the corner pairs into $27DA8 (Op2_AimAtArgPoint, absolute); Scr5_023156 +$17C/+$186 (x $B4, y $203C, delays $F0/$780, group action 1), right after the two SpawnCircuit entries (y $1FC4); attack vector 1 = $2ADFE (Fx_SpawnAtOfsAngle at (-$10, -$B/+$B), heading $200), shared with Scr2_02ABD2; TAS 52007-52147
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnCircuitB:
	lea.l	(Scr2_02ACF0).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
