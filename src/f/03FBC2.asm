; ---------------------------------------------------------------------------
; Boss_Hatter_SpawnRoadSplitLeft - road schedule spawn kind 2: a road object (body $03DE16) that after $4B0 ticks bursts into three pieces flying left (cruise x -$A00)
; In:  d4 = the lane's road x, d5 = y (replaced by the road distance); a4 = the boss's VM block
; Out: a6 = the object, d0 = 0 (the placement); a full pool quits the caller (Obj_SpawnEnemyOrQuit returns past this)
; Evidence: lea script / jsr Obj_SpawnEnemyOrQuit / jmp Stage3_RoadPlaceAtHorizon(pc); named by the lea at $03DD62 (Boss_Hatter_RoadCmdSpawn2, entry 10 of Boss_Hatter_RoadCmdTbl); the Hatter's road schedules (StartCoroutine2Rel targets $03F58C-$03FAB0 from the boss scripts $03E11A-$03F25A) use these kinds; the corpus ran kind 0 only; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Hatter_SpawnRoadSplitLeft:
	lea.l	(Scr2_03FBD2).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage3_RoadPlaceAtHorizon(pc)
