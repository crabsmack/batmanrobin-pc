; ---------------------------------------------------------------------------
; Boss_Hatter_SpawnRoadHazardNear - road schedule spawn kind 7: the kind-0 road hazard (Scr2_03FB08: StopCoroutine0, body $03DE16) placed nearer than the horizon (Stage3_RoadPlaceNear, distance $840)
; In:  d4 = the lane's road x, d5 = y (replaced by the road distance); a4 = the boss's VM block
; Out: a6 = the object, d0 = 0 (the placement); a full pool quits the caller (Obj_SpawnEnemyOrQuit returns past this)
; Evidence: lea script / jsr Obj_SpawnEnemyOrQuit / jmp Stage3_RoadPlaceNear(pc); named by the lea at $03DD8A (Boss_Hatter_RoadCmdSpawn7, entry 15 of Boss_Hatter_RoadCmdTbl); the Hatter's road schedules (StartCoroutine2Rel targets $03F58C-$03FAB0 from the boss scripts $03E11A-$03F25A) use these kinds; the corpus ran kind 0 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_SpawnRoadHazardNear:
	lea.l	(Scr2_03FB08).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage3_RoadPlaceNear(pc)
