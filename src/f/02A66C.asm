; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnWaypointsD - wave spawner: WaypointsC mirrored vertically (Scr2_02A67C): relative waypoints (dx, dy) (-$28, 0), (-$28, +$32), (-5, -6), twice (+$7C, -$94), then (+$1F4, -$258) off the screen, upper gun first (vars 0/1 = 0); fires 3 rounds $F0 apart, then waits $480
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs2 word pairs (y, x) into Op2_AimAtTargetArgOffset (Arg0 = dx, Arg1 = dy); C's pairs differ only in the y sign; Scr5_022ADE +$190/+$19A and Scr5_022C7E +$A2/+$AC; TAS 40941-41032
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnWaypointsD:
	lea.l	(Scr2_02A67C).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
