; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnCircuit - wave spawner: a red gun drone (Scr2_02ABD2) that flies 30 px left of its entry, then loops twice round a circuit through the absolute points (x $3C, y $13C), (x $78, y $50), (x $50, y $13C), (x $3C, y $50), each followed by two short relative legs (e.g. (dx, dy) (+$28, +$14), (+$28, -$14)), then moves by one of 4 random relative offsets down-left ((-$50, +$3C), (-$28, +$28), (-$50, +$50), (-$28, +$78)), 40 px further left, follows the player's height $F00 ticks and leaves
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs2 words are (dy, dx); the corner pairs go to $27DA8 (Op2_AimAtArgPoint, absolute x/y), the other legs to $27D90/$27D98 (Op2_AimAtTargetArgOffset after Op2_TargetSelf), the last leg to vector 2 $27D1A (Op2_CruiseToPlayerY); Scr5_023156 +$168/+$172 (x $B4, y $1FC4, delays $F0/$780); Op2_SetCounter1 2 / Op2_LoopCounter1 round 10 waypoints, Op2_SwitchRandom over 4 exits; body $27F70; TAS 57398-57538
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnCircuit:
	lea.l	(Scr2_02ABD2).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
