; ---------------------------------------------------------------------------
; Boss_Hatter_SpawnRoadSplitLeftSoon - road schedule spawn kind 6: a road object (body $03DE16) that after only $78 ticks bursts into three pieces flying left (cruise x -$500)
; In:  d4 = the lane's road x, d5 = y (replaced by the road distance); a4 = the boss's VM block
; Out: a6 = the object, d0 = 0 (the placement); a full pool quits the caller (Obj_SpawnEnemyOrQuit returns past this)
; Evidence: lea script / jsr Obj_SpawnEnemyOrQuit / jmp Stage3_RoadPlaceAtHorizon(pc); named by the lea at $03DD82 (Boss_Hatter_RoadCmdSpawn6, entry 14 of Boss_Hatter_RoadCmdTbl); the Hatter's road schedules (StartCoroutine2Rel targets $03F58C-$03FAB0 from the boss scripts $03E11A-$03F25A) use these kinds; the corpus ran kind 0 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_SpawnRoadSplitLeftSoon:
	lea.l	(Scr2_03FD0E).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage3_RoadPlaceAtHorizon(pc)
