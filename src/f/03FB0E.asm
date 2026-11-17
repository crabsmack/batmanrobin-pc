; ---------------------------------------------------------------------------
; Boss_Hatter_SpawnRoadHazard - road schedule spawn kind 0: spawns Scr2_03FB1E (enemy class), a road hazard that rides the road from its far end toward the players, and puts it at the horizon (Stage3_RoadPlaceAtHorizon)
; In:  d4 = road x of the lane
; Out: a6 = the object, d0 = 0 (quits the caller when the pool is full)
; Evidence: pointer in Boss_Hatter_RoadCmdSpawn0; Obj_SpawnEnemyOrQuit; Scr2_03FB1E: Op2_StopCoroutine0, Op2_Goto $03DE16 (the body); TAS f114753/f114833; png f114930: a small red-framed box at the road's far end (what it is: not identified)
; ---------------------------------------------------------------------------
Boss_Hatter_SpawnRoadHazard:
	lea.l	(Scr2_03FB1E).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage3_RoadPlaceAtHorizon(pc)
