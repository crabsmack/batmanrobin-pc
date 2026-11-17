; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnWaypointsC - wave spawner: a gun drone (Scr2_02A5FE) on a course of relative waypoints (dx, dy) (-$28, 0), (-$28, -$32), (-5, +6), twice (+$7C, +$94), then (+$1F4, +$258) off the screen down-right
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs2 words are (dy, dx), into Op2_AimAtTargetArgOffset ($27D90/$27D98); Scr5_022ADE +$17C/+$186 and Scr5_022C7E +$8E/+$98 (x $AA, y $2000), paired with WaypointsD; TAS 40732-41548
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnWaypointsC:
	lea.l	(Scr2_02A5FE).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
